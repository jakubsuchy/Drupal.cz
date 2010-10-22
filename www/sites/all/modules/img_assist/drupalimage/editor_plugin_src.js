/* Import plugin specific language pack */
tinyMCE.importPluginLanguagePack('drupalimage', 'en');

var TinyMCE_DrupalImagePlugin = {
	getInfo : function() {
		return {
			longname : 'DrupalImage',
			author : 'Benjamin Shell',
			authorurl : 'http://www.benjaminshell.com',
			infourl : 'http://drupal.org/project/img_assist',
			version : tinyMCE.majorVersion + "." + tinyMCE.minorVersion
		};
	},

	initInstance : function(inst) {
		if (!tinyMCE.settings['drupalimage_skip_plugin_css'])
			tinyMCE.importCSS(inst.getDoc(), tinyMCE.baseURL + "/plugins/drupalimage/drupalimage.css");
	},

	getControlHTML : function(cn) {
		switch (cn) {
			case "drupalimage":
				return tinyMCE.getButtonHTML(cn, 'lang_drupalimage_desc', '{$pluginurl}/images/drupalimage.gif', 'mceDrupalImage');
		}

		return "";
	},

	execCommand : function(editor_id, element, command, user_interface, value) {
		// Handle commands
		switch (command) {
			case "mceDrupalImage":
				var name = "";
				var nid = "", alt = "", captionTitle = "", captionDesc = "", link = "", align = "", width = "", height = "";
				var action = "insert";
				var template = new Array();
				var inst = tinyMCE.getInstanceById(editor_id);
				var focusElm = inst.getFocusElement();
				
				// get base url
				var base_url = tinyMCE.baseURL;
				base_url = base_url.substring(0, base_url.indexOf('modules'));
				
				template['file'] = base_url + 'index.php?q=img_assist/load/tinymce';
				template['width'] = 600;
				template['height'] = 350;
				template['html'] = false;
				
				// Is selection a image
				if (focusElm != null && focusElm.nodeName.toLowerCase() == "img") {
					name = tinyMCE.getAttrib(focusElm, 'class');

					if (name.indexOf('mceItemDrupalImage') == -1) // Not a DrupalImage
						return true;

					// Get the rest of the DrupalImage attributes
					align = tinyMCE.getAttrib(focusElm, 'align');
					width = tinyMCE.getAttrib(focusElm, 'width');
					height = tinyMCE.getAttrib(focusElm, 'height');
					alt = decodeURIComponent(tinyMCE.getAttrib(focusElm, 'title')); // using 'title' because this doesn't seem to work with 'alt'
					var miscAttribs = TinyMCE_DrupalImagePlugin._parsePipeAttributes(alt);	// parse the deliminated attributes in the alt tag
					nid = miscAttribs['nid'];
					captionTitle = miscAttribs['title'];
					captionDesc = miscAttribs['desc'];
					link = miscAttribs['link'];
					
					action = "update";
				}
				
				//WARNING: "resizable : 'yes'" below is painfully important otherwise
				// tinymce will try to open a new window in IE using showModalDialog().
				// And for some reason showModalDialog() doesn't respect the target="_top"
				// attribute.
				tinyMCE.openWindow(template, {editor_id : editor_id, nid : nid, captionTitle : captionTitle, captionDesc : captionDesc, link : link, align : align, width : width, height : height, action : action});
				//tinyMCE.openWindow(template, {editor_id : editor_id, nid : nid, captionTitle : captionTitle, captionDesc : captionDesc, link : link, align : align, width : width, height : height, action : action, resizable : 'yes', scrollbars : 'yes'});
			return true;
	   }

	   // Pass to next handler in chain
	   return false;
	},
	
	cleanup : function(type, content) {
		switch (type) {
			case "insert_to_editor_dom":
				break;
			case "get_from_editor_dom":
				break;
			case "insert_to_editor": // called when TinyMCE loads existing data or when updating code using Edit HTML Source plugin 
				// Parse all drupalimage filter tags and replace them with image placeholders
				var startPos = 0;
				var index = 0;
				while ((startPos = content.indexOf('[img_assist|', startPos)) != -1) {
					// Find end of object
					var endPos = content.indexOf(']', startPos);
					var attribs = TinyMCE_DrupalImagePlugin._parsePipeAttributes(content.substring(startPos + 12, endPos));
					endPos++;
					
					// the TinyMCE_DrupalImagePlugin._parsePipeAttributes() function parses the piped string completely
					// but in this case we want to keep the nid, title, and desc in piped format
					// so we have to rebuild a partial piped string
					var miscAttribs = 'nid=' + attribs['nid'] + '|title=' + attribs['title'] + '|desc=' + attribs['desc'] + '|link=' + attribs['link'];
					// ordinarily piped strings wouldn't need to have HTML entities converted, but we are building an 
					// HTML tag that just happens to use a piped string as one of its' attribute values. The
					// easiest way to take care of HTML entities is with the Javascript escape() function.
					// It escapes more than necessary, but that's okay.  We'll use unescape() to go back when
					// we need to.
					miscAttribs = encodeURIComponent(miscAttribs); // escape(miscAttribs);
					
					// Insert image
					var contentAfter = content.substring(endPos);
					content = content.substring(0, startPos);
					// Reference: these are the default parameters that are valid for the TinyMCE image tags:
					// img[class|src|border=0|alt|title|hspace|vspace|width|height|align]
					content += '<img src="' + (tinyMCE.getParam("theme_href") + '/images/spacer.gif') + '"';
					content += ' width="' + attribs["width"] + '" height="' + attribs["height"] + '" align="' + attribs['align'] + '"';
					content += ' alt="' + miscAttribs + '" title="' + miscAttribs + '" name="mceItemDrupalImage" class="mceItemDrupalImage" />';
					content += contentAfter;
					index++;
					
					startPos++;
				}
				break;

			case "get_from_editor": // called when TinyMCE exits or when the Edit HTML Source plugin is clicked
				// Parse all image placeholders and replace them with drupalimage filter tags
				var startPos = -1;
				while ((startPos = content.indexOf('<img', startPos+1)) != -1) {
					var endPos = content.indexOf('/>', startPos);
					var attribs = TinyMCE_DrupalImagePlugin._parseHTMLAttributes(content.substring(startPos + 4, endPos));
					endPos += 2;
					
					// Is not drupalimage, skip it
					if (attribs['name'] != "mceItemDrupalImage") {
						continue;
					}
					
					// Insert drupalimage filter code
					//  At this point all attribute values should have any pipes | or closing square brackets ]
					//  escaped with backslashes.  When this filter code is parsed, it will look for the closing 
					//  bracket to find the end and the pipe symbol to explode the rest of the attributes.  However,
					//  since we just parsed from HTML tag, HTML entities should be unescaped at this time.
					var miscAttribs = decodeURIComponent(attribs["alt"]);
					
					var contentBefore = content.substring(0, startPos);
					var contentAfter = content.substring(endPos);
					var drupalHTML = '';
					drupalHTML += '[img_assist|' + miscAttribs + '|align=' + attribs["align"];
					drupalHTML += '|width=' + attribs["width"] + '|height=' + attribs["height"] + ']';
					content = contentBefore + drupalHTML + contentAfter;
				}
				break;
		}

		// Pass through to next handler in chain
		return content;
	},
	
	handleNodeChange : function(editor_id, node, undo_index, undo_levels, visual_aid, any_selection) {
		if (node == null)
			return;

		do {
			// This code looks at the name of the image to see if the drupalimage button should be selected.
			// However, by default 'name' is not accepted by TinyMCE as a parameter for the img tag, so it must
			// be added using the initialization string.  As far as THIS code goes, it could look at 'className' 
			// instead, therefore avoiding this requirement, however the regular image button looks at the 
			// 'name' value to see if it starts with 'mce_'.  If it does, it considers it an internal image and 
			// does not highlight the regular image button.  If 'className' is used here instead, BOTH buttons
			// highlight when a drupalimage is selected.
			if (node.nodeName == "IMG" && tinyMCE.getAttrib(node, 'class').indexOf('mceItemDrupalImage') == 0) {
				tinyMCE.switchClass(editor_id + '_drupalimage', 'mceButtonSelected');
				return true;
			}
		} while ((node = node.parentNode));

		tinyMCE.switchClass(editor_id + '_drupalimage', 'mceButtonNormal');

		return true;
	},
	
	// pipes | must be escaped with a backslash like this: \|
	// note: values also cannot contain ] because the functions that call 
	// this function use the ] symbol to find the end of the drupalimage filter code
	_parsePipeAttributes : function(attribute_string) {
		var attributes = new Array();
		var keyvalue_arr = new Array();
		// if it weren't for the escaping, the regExp string would look like this:
		// var regExp = new RegExp("([a-zA-Z]*)=([^\|]*)", "g"); // doesn't work without global (g)
		var regExp = new RegExp("([a-zA-Z]*)=([^|](?:\\.|[^\\|]*)*)*", "g"); // doesn't work without global (g)
		var matches = attribute_string.match(regExp);
		for (var i = 0; i < matches.length; i++ ) {
			keyvalue_arr = matches[i].split('=');
			attributes[keyvalue_arr[0]] = keyvalue_arr[1];
		}
		return attributes;
	},
	
	/* 
	 * Parses HTML attributes into a key=>value array.  Take a look at the example strings and see how
	 * standard HTML entities within any value, such as the title and desc (which are combined in the
	 * alt tag as a piped string) need to be converted to: &quot; &amp; &lt; &gt;
	 * simple example string: 
	 *     name="mceItemDrupalImage" width="200" height="150" src="/images/spacer.gif" 
	 *     alt="nid=123|title=My Photos|desc=" class="mceItemDrupalImage" align="right"
	 * advanced example string: 
	 *     name="mceItemDrupalImage" width="200" height="150" src="/images/spacer.gif" 
	 *     alt="nid=123|title=&quot;To be or not to be&quot;|desc=That is the question." class="mceItemDrupalImage" align="right"
	 * Any pipes | or closing brackets would also be a problem, not for this parsing function, but 
	 * when parsing the pipe deliminated string.  These characters need to be escaped with a backslash.
	 * Unlike the quotes, this cannot be accomplished automatically within this TinyMCE plugin.  Any  
	 * user or Drupal module that inserts drupalimage filter strings in a post, whether using TinyMCE
	 * or not, must backslash any pipes or closing brackets.
	*/
	_parseHTMLAttributes : function(attribute_string) {
		var attributes = new Array();
		var innerMatches = new Array();
		var regExp = '([a-zA-Z0-9]+)[\s]*=[\s]*"([^"](?:\\.|[^\\"]*)*)"';
	
		var outerRegExp = new RegExp(regExp, "g"); // doesn't work without global (g)
		var outerMatches = attribute_string.match(outerRegExp);
		var innerRegExp = new RegExp(regExp); // doesn't work with global (g)
		for (var i = 0; i < outerMatches.length; i++ ) {
			innerMatches = innerRegExp.exec(outerMatches[i]);
			attributes[innerMatches[1]] = innerMatches[2];
		}
		return attributes;
	}
};

tinyMCE.addPlugin("drupalimage", TinyMCE_DrupalImagePlugin);
