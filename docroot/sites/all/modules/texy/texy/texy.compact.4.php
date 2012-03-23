<?php
/**
 * Texy! - web text markup-language (for PHP 4)
 * --------------------------------------------
 *
 * Copyright (c) 2004, 2008 David Grudl aka -dgx- (http://www.dgx.cz)
 *
 * This source file is subject to the GNU GPL license that is bundled
 * with this package in the file license.txt.
 *
 * For more information please see http://texy.info/
 *
 * @author     David Grudl
 * @copyright  Copyright (c) 2004, 2008 David Grudl
 * @license    GNU GENERAL PUBLIC LICENSE version 2 or 3
 * @version    2.0 BETA 2 (Revision: 201, Date: 2008/02/01 03:00:29)
 * @package    Texy
 * @link       http://texy.info/
 */

define('TEXY_VERSION','2.0 FOR PHP4 BETA 2 (Revision: 201, Date: 2008/02/01 03:00:29)');if(PHP_VERSION>=5){error_reporting(error_reporting()&~E_STRICT);}if(!class_exists('NObject4')){if(PHP_VERSION<5){eval('
    function clone($obj)
    {
        foreach($obj as $key => $value) {
            $obj->$key = & $value;               // reference to new variable
            $GLOBALS[\'$$HIDDEN$$\'][] = & $value; // and generate reference
            unset($value);
        }

        // call $obj->__clone()
        if (is_callable(array(&$obj, \'__clone\'))) $obj->__clone();

        return $obj;
    }



    function throw($e)
    {
        trigger_error($e->message, E_USER_ERROR);
        die();
    }
    ');class
Exception{var$message;var$code;var$trace;function
Exception($message=NULL,$code=0){$this->message=$message;$this->code=$code;$this->trace=debug_backtrace();}}}class
NObject4{function
__construct(){}function
NObject4(){foreach($this
as$key=>$foo)$GLOBALS['$$HIDDEN$$'][]=&$this->$key;$args=func_get_args();call_user_func_array(array(&$this,'__construct'),$args);}function
getClass(){return
get_class($this);}function
getReflection(){return
new
ReflectionObject($this);}}}define('TEXY_ALL',TRUE);define('TEXY_NONE',FALSE);define('TEXY_CONTENT_MARKUP',"\x17");define('TEXY_CONTENT_REPLACED',"\x16");define('TEXY_CONTENT_TEXTUAL',"\x15");define('TEXY_CONTENT_BLOCK',"\x14");$GLOBALS['Texy::$strictDTD']=FALSE;$GLOBALS['Texy::$advertisingNotice']='once';class
Texy
extends
NObject4{var$encoding='utf-8';var$allowed=array();var$allowedTags;var$allowedClasses=TEXY_ALL;var$allowedStyles=TEXY_ALL;var$tabWidth=8;var$obfuscateEmail=TRUE;var$urlSchemeFilters=NULL;var$mergeLines=TRUE;var$summary=array('images'=>array(),'links'=>array(),'preload'=>array(),);var$styleSheet='';var$alignClasses=array('left'=>NULL,'right'=>NULL,'center'=>NULL,'justify'=>NULL,'top'=>NULL,'middle'=>NULL,'bottom'=>NULL,);var$nontextParagraph='div';var$scriptModule;var$paragraphModule;var$htmlModule;var$imageModule;var$linkModule;var$phraseModule;var$emoticonModule;var$blockModule;var$headingModule;var$horizLineModule;var$blockQuoteModule;var$listModule;var$tableModule;var$figureModule;var$typographyModule;var$longWordsModule;var$htmlOutputModule;var$linePatterns=array();var$_linePatterns;var$blockPatterns=array();var$_blockPatterns;var$postHandlers=array();var$DOM;var$marks=array();var$_classes,$_styles;var$processing;var$handlers=array();var$cleaner;var$xhtml;function
__construct(){TexyHtml::initDTD($GLOBALS['Texy::$strictDTD']);foreach($GLOBALS['TexyHtml::$dtd']as$tag=>$dtd){$this->allowedTags[$tag]=TEXY_ALL;}$this->loadModules();$this->cleaner=&$this->htmlOutputModule;$this->xhtml=&$this->htmlOutputModule->xhtml;$link=new
TexyLink('http://texy.info/');$link->modifier->title='The best text -> HTML converter and formatter';$link->label='Texy!';$this->linkModule->addReference('texy',$link);$link=new
TexyLink('http://www.google.com/search?q=%s');$this->linkModule->addReference('google',$link);$link=new
TexyLink('http://en.wikipedia.org/wiki/Special:Search?search=%s');$this->linkModule->addReference('wikipedia',$link);}function
loadModules(){$this->scriptModule=new
TexyScriptModule($this);$this->htmlModule=new
TexyHtmlModule($this);$this->imageModule=new
TexyImageModule($this);$this->phraseModule=new
TexyPhraseModule($this);$this->linkModule=new
TexyLinkModule($this);$this->emoticonModule=new
TexyEmoticonModule($this);$this->paragraphModule=new
TexyParagraphModule($this);$this->blockModule=new
TexyBlockModule($this);$this->figureModule=new
TexyFigureModule($this);$this->horizLineModule=new
TexyHorizLineModule($this);$this->blockQuoteModule=new
TexyBlockQuoteModule($this);$this->tableModule=new
TexyTableModule($this);$this->headingModule=new
TexyHeadingModule($this);$this->listModule=new
TexyListModule($this);$this->typographyModule=new
TexyTypographyModule($this);$this->longWordsModule=new
TexyLongWordsModule($this);$this->htmlOutputModule=new
TexyHtmlOutputModule($this);}function
registerLinePattern($handler,$pattern,$name){if(!isset($this->allowed[$name]))$this->allowed[$name]=TRUE;$this->linePatterns[$name]=array('handler'=>$handler,'pattern'=>$pattern,);}function
registerBlockPattern($handler,$pattern,$name){if(!isset($this->allowed[$name]))$this->allowed[$name]=TRUE;$this->blockPatterns[$name]=array('handler'=>$handler,'pattern'=>$pattern.'m',);}function
registerPostLine($handler,$name){if(!isset($this->allowed[$name]))$this->allowed[$name]=TRUE;$this->postHandlers[$name]=$handler;}function
process($text,$singleLine=FALSE){if($this->processing){throw(new
InvalidStateException('Processing is in progress yet.'));}$this->marks=array();$this->processing=TRUE;if(is_array($this->allowedClasses))$this->_classes=array_flip($this->allowedClasses);else$this->_classes=$this->allowedClasses;if(is_array($this->allowedStyles))$this->_styles=array_flip($this->allowedStyles);else$this->_styles=$this->allowedStyles;$text=TexyUtf::toUtf($text,$this->encoding);$text=Texy::normalize($text);$this->tabWidth=max(1,(int)$this->tabWidth);while(strpos($text,"\t")!==FALSE){$text=preg_replace_callback('#^(.*)\t#mU',array($this,'tabCb'),$text);}$this->invokeHandlers('beforeParse',array($this,&$text,$singleLine));$this->_linePatterns=$this->linePatterns;$this->_blockPatterns=$this->blockPatterns;foreach($this->_linePatterns
as$name=>$foo){if(empty($this->allowed[$name]))unset($this->_linePatterns[$name]);}foreach($this->_blockPatterns
as$name=>$foo){if(empty($this->allowed[$name]))unset($this->_blockPatterns[$name]);}$this->DOM=TexyHtml::el();if($singleLine){$this->DOM->parseLine($this,$text);}else{$this->DOM->parseBlock($this,$text);}$this->invokeHandlers('afterParse',array($this,$this->DOM,$singleLine));$html=$this->DOM->toHtml($this);if($GLOBALS['Texy::$advertisingNotice']){$html.="\n<!-- by Texy2! -->";if($GLOBALS['Texy::$advertisingNotice']==='once'){$GLOBALS['Texy::$advertisingNotice']=FALSE;}}$this->processing=FALSE;return
TexyUtf::utf2html($html,$this->encoding);}function
processTypo($text){$text=TexyUtf::toUtf($text,$this->encoding);$text=Texy::normalize($text);$this->typographyModule->beforeParse($this,$text);$text=$this->typographyModule->postLine($text);return
TexyUtf::utf2html($text,$this->encoding);}function
toText(){if(!$this->DOM){throw(new
InvalidStateException('Call $texy->process() first.'));}return
TexyUtf::utfTo($this->DOM->toText($this),$this->encoding);}function
stringToHtml($s){$s=Texy::unescapeHtml($s);$blocks=explode(TEXY_CONTENT_BLOCK,$s);foreach($this->postHandlers
as$name=>$handler){if(empty($this->allowed[$name]))continue;foreach($blocks
as$n=>$s){if($n
%
2===0&&$s!==''){$blocks[$n]=call_user_func($handler,$s);}}}$s=implode(TEXY_CONTENT_BLOCK,$blocks);$s=Texy::escapeHtml($s);$s=$this->unProtect($s);$this->invokeHandlers('postProcess',array($this,&$s));$s=Texy::unfreezeSpaces($s);return$s;}function
stringToText($s){$save=$this->htmlOutputModule->lineWrap;$this->htmlOutputModule->lineWrap=FALSE;$s=$this->stringToHtml($s);$this->htmlOutputModule->lineWrap=$save;$s=preg_replace('#<(script|style)(.*)</\\1>#Uis','',$s);$s=strip_tags($s);$s=preg_replace('#\n\s*\n\s*\n[\n\s]*\n#',"\n\n",$s);$s=Texy::unescapeHtml($s);$s=strtr($s,array("\xC2\xAD"=>'',"\xC2\xA0"=>' ',));return$s;}function
addHandler($event,$callback){if(!is_callable($callback)){throw(new
InvalidArgumentException("Invalid callback"));}$this->handlers[$event][]=$callback;}function
invokeAroundHandlers($event,$parser,$args){if(!isset($this->handlers[$event]))return
FALSE;$invocation=new
TexyHandlerInvocation($this->handlers[$event],$parser,$args);$res=$invocation->proceed();$invocation->free();return$res;}function
invokeHandlers($event,$args){if(!isset($this->handlers[$event]))return;foreach($this->handlers[$event]as$handler){call_user_func_array($handler,$args);}}function
freezeSpaces($s){return
strtr($s," \t\r\n","\x01\x02\x03\x04");}function
unfreezeSpaces($s){return
strtr($s,"\x01\x02\x03\x04"," \t\r\n");}function
normalize($s){$s=str_replace("\r\n","\n",$s);$s=strtr($s,"\r","\n");$s=preg_replace('#[\x00-\x08\x0B-\x1F]+#','',$s);$s=preg_replace("#[\t ]+$#m",'',$s);$s=trim($s,"\n");return$s;}function
webalize($s,$charlist=NULL){$s=TexyUtf::utf2ascii($s);$s=strtolower($s);if($charlist)$charlist=preg_quote($charlist,'#');$s=preg_replace('#[^a-z0-9'.$charlist.']+#','-',$s);$s=trim($s,'-');return$s;}function
escapeHtml($s){return
str_replace(array('&','<','>'),array('&amp;','&lt;','&gt;'),$s);}function
unescapeHtml($s){if(strpos($s,'&')===FALSE)return$s;if(PHP_VERSION>=5)return
html_entity_decode($s,ENT_QUOTES,'UTF-8');static$entity=array('&AElig;'=>'&#198;','&Aacute;'=>'&#193;','&Acirc;'=>'&#194;','&Agrave;'=>'&#192;','&Alpha;'=>'&#913;','&Aring;'=>'&#197;','&Atilde;'=>'&#195;','&Auml;'=>'&#196;','&Beta;'=>'&#914;','&Ccedil;'=>'&#199;','&Chi;'=>'&#935;','&Dagger;'=>'&#8225;','&Delta;'=>'&#916;','&ETH;'=>'&#208;','&Eacute;'=>'&#201;','&Ecirc;'=>'&#202;','&Egrave;'=>'&#200;','&Epsilon;'=>'&#917;','&Eta;'=>'&#919;','&Euml;'=>'&#203;','&Gamma;'=>'&#915;','&Iacute;'=>'&#205;','&Icirc;'=>'&#206;','&Igrave;'=>'&#204;','&Iota;'=>'&#921;','&Iuml;'=>'&#207;','&Kappa;'=>'&#922;','&Lambda;'=>'&#923;','&Mu;'=>'&#924;','&Ntilde;'=>'&#209;','&Nu;'=>'&#925;','&OElig;'=>'&#338;','&Oacute;'=>'&#211;','&Ocirc;'=>'&#212;','&Ograve;'=>'&#210;','&Omega;'=>'&#937;','&Omicron;'=>'&#927;','&Oslash;'=>'&#216;','&Otilde;'=>'&#213;','&Ouml;'=>'&#214;','&Phi;'=>'&#934;','&Pi;'=>'&#928;','&Prime;'=>'&#8243;','&Psi;'=>'&#936;','&Rho;'=>'&#929;','&Scaron;'=>'&#352;','&Sigma;'=>'&#931;','&THORN;'=>'&#222;','&Tau;'=>'&#932;','&Theta;'=>'&#920;','&Uacute;'=>'&#218;','&Ucirc;'=>'&#219;','&Ugrave;'=>'&#217;','&Upsilon;'=>'&#933;','&Uuml;'=>'&#220;','&Xi;'=>'&#926;','&Yacute;'=>'&#221;','&Yuml;'=>'&#376;','&Zeta;'=>'&#918;','&aacute;'=>'&#225;','&acirc;'=>'&#226;','&acute;'=>'&#180;','&aelig;'=>'&#230;','&agrave;'=>'&#224;','&alefsym;'=>'&#8501;','&alpha;'=>'&#945;','&amp;'=>'&#38;','&and;'=>'&#8743;','&ang;'=>'&#8736;','&apos;'=>'&#39;','&aring;'=>'&#229;','&asymp;'=>'&#8776;','&atilde;'=>'&#227;','&auml;'=>'&#228;','&bdquo;'=>'&#8222;','&beta;'=>'&#946;','&brvbar;'=>'&#166;','&bull;'=>'&#8226;','&cap;'=>'&#8745;','&ccedil;'=>'&#231;','&cedil;'=>'&#184;','&cent;'=>'&#162;','&chi;'=>'&#967;','&circ;'=>'&#710;','&clubs;'=>'&#9827;','&cong;'=>'&#8773;','&copy;'=>'&#169;','&crarr;'=>'&#8629;','&cup;'=>'&#8746;','&curren;'=>'&#164;','&dArr;'=>'&#8659;','&dagger;'=>'&#8224;','&darr;'=>'&#8595;','&deg;'=>'&#176;','&delta;'=>'&#948;','&diams;'=>'&#9830;','&divide;'=>'&#247;','&eacute;'=>'&#233;','&ecirc;'=>'&#234;','&egrave;'=>'&#232;','&empty;'=>'&#8709;','&emsp;'=>'&#8195;','&ensp;'=>'&#8194;','&epsilon;'=>'&#949;','&equiv;'=>'&#8801;','&eta;'=>'&#951;','&eth;'=>'&#240;','&euml;'=>'&#235;','&euro;'=>'&#8364;','&exist;'=>'&#8707;','&fnof;'=>'&#402;','&forall;'=>'&#8704;','&frac12;'=>'&#189;','&frac14;'=>'&#188;','&frac34;'=>'&#190;','&frasl;'=>'&#8260;','&gamma;'=>'&#947;','&ge;'=>'&#8805;','&gt;'=>'&#62;','&hArr;'=>'&#8660;','&harr;'=>'&#8596;','&hearts;'=>'&#9829;','&hellip;'=>'&#8230;','&iacute;'=>'&#237;','&icirc;'=>'&#238;','&iexcl;'=>'&#161;','&igrave;'=>'&#236;','&image;'=>'&#8465;','&infin;'=>'&#8734;','&int;'=>'&#8747;','&iota;'=>'&#953;','&iquest;'=>'&#191;','&isin;'=>'&#8712;','&iuml;'=>'&#239;','&kappa;'=>'&#954;','&lArr;'=>'&#8656;','&lambda;'=>'&#955;','&lang;'=>'&#9001;','&laquo;'=>'&#171;','&larr;'=>'&#8592;','&lceil;'=>'&#8968;','&ldquo;'=>'&#8220;','&le;'=>'&#8804;','&lfloor;'=>'&#8970;','&lowast;'=>'&#8727;','&loz;'=>'&#9674;','&lrm;'=>'&#8206;','&lsaquo;'=>'&#8249;','&lsquo;'=>'&#8216;','&lt;'=>'&#60;','&macr;'=>'&#175;','&mdash;'=>'&#8212;','&micro;'=>'&#181;','&middot;'=>'&#183;','&minus;'=>'&#8722;','&mu;'=>'&#956;','&nabla;'=>'&#8711;','&nbsp;'=>'&#160;','&ndash;'=>'&#8211;','&ne;'=>'&#8800;','&ni;'=>'&#8715;','&not;'=>'&#172;','&notin;'=>'&#8713;','&nsub;'=>'&#8836;','&ntilde;'=>'&#241;','&nu;'=>'&#957;','&oacute;'=>'&#243;','&ocirc;'=>'&#244;','&oelig;'=>'&#339;','&ograve;'=>'&#242;','&oline;'=>'&#8254;','&omega;'=>'&#969;','&omicron;'=>'&#959;','&oplus;'=>'&#8853;','&or;'=>'&#8744;','&ordf;'=>'&#170;','&ordm;'=>'&#186;','&oslash;'=>'&#248;','&otilde;'=>'&#245;','&otimes;'=>'&#8855;','&ouml;'=>'&#246;','&para;'=>'&#182;','&part;'=>'&#8706;','&permil;'=>'&#8240;','&perp;'=>'&#8869;','&phi;'=>'&#966;','&pi;'=>'&#960;','&piv;'=>'&#982;','&plusmn;'=>'&#177;','&pound;'=>'&#163;','&prime;'=>'&#8242;','&prod;'=>'&#8719;','&prop;'=>'&#8733;','&psi;'=>'&#968;','&quot;'=>'&#34;','&rArr;'=>'&#8658;','&radic;'=>'&#8730;','&rang;'=>'&#9002;','&raquo;'=>'&#187;','&rarr;'=>'&#8594;','&rceil;'=>'&#8969;','&rdquo;'=>'&#8221;','&real;'=>'&#8476;','&reg;'=>'&#174;','&rfloor;'=>'&#8971;','&rho;'=>'&#961;','&rlm;'=>'&#8207;','&rsaquo;'=>'&#8250;','&rsquo;'=>'&#8217;','&sbquo;'=>'&#8218;','&scaron;'=>'&#353;','&sdot;'=>'&#8901;','&sect;'=>'&#167;','&shy;'=>'&#173;','&sigma;'=>'&#963;','&sigmaf;'=>'&#962;','&sim;'=>'&#8764;','&spades;'=>'&#9824;','&sub;'=>'&#8834;','&sube;'=>'&#8838;','&sum;'=>'&#8721;','&sup1;'=>'&#185;','&sup2;'=>'&#178;','&sup3;'=>'&#179;','&sup;'=>'&#8835;','&supe;'=>'&#8839;','&szlig;'=>'&#223;','&tau;'=>'&#964;','&there4;'=>'&#8756;','&theta;'=>'&#952;','&thetasym;'=>'&#977;','&thinsp;'=>'&#8201;','&thorn;'=>'&#254;','&tilde;'=>'&#732;','&times;'=>'&#215;','&trade;'=>'&#8482;','&uArr;'=>'&#8657;','&uacute;'=>'&#250;','&uarr;'=>'&#8593;','&ucirc;'=>'&#251;','&ugrave;'=>'&#249;','&uml;'=>'&#168;','&upsih;'=>'&#978;','&upsilon;'=>'&#965;','&uuml;'=>'&#252;','&weierp;'=>'&#8472;','&xi;'=>'&#958;','&yacute;'=>'&#253;','&yen;'=>'&#165;','&yuml;'=>'&#255;','&zeta;'=>'&#950;','&zwj;'=>'&#8205;','&zwnj;'=>'&#8204;',);$s=str_replace(array_keys($entity),array_values($entity),$s);$s=preg_replace_callback('#&(\\#x[0-9a-fA-F]+|\\#[0-9]+);#',array('Texy','_entityCb'),$s);return$s;}function
outdent($s){$s=trim($s,"\n");$spaces=strspn($s,' ');if($spaces)return
preg_replace("#^ {1,$spaces}#m",'',$s);return$s;}function
_entityCb($matches){list(,$entity)=$matches;$ord=($entity{1}=='x')?hexdec(substr($entity,2)):(int)substr($entity,1);if($ord<128)return
chr($ord);if($ord<2048)return
chr(($ord>>6)+192).chr(($ord&63)+128);if($ord<65536)return
chr(($ord>>12)+224).chr((($ord>>6)&63)+128).chr(($ord&63)+128);if($ord<2097152)return
chr(($ord>>18)+240).chr((($ord>>12)&63)+128).chr((($ord>>6)&63)+128).chr(($ord&63)+128);return$match;}function
protect($child,$contentType){if($child==='')return'';$key=$contentType.strtr(base_convert(count($this->marks),10,8),'01234567',"\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F").$contentType;$this->marks[$key]=$child;return$key;}function
unProtect($html){return
strtr($html,$this->marks);}function
checkURL($URL,$type){if(!empty($this->urlSchemeFilters[$type])&&preg_match('#'.TEXY_URLSCHEME.'#iA',$URL)&&!preg_match($this->urlSchemeFilters[$type],$URL))return
FALSE;return
TRUE;}function
isRelative($URL){return!preg_match('#'.TEXY_URLSCHEME.'|[\#/?]#iA',$URL);}function
prependRoot($URL,$root){if($root==NULL||!Texy::isRelative($URL))return$URL;return
rtrim($root,'/\\').'/'.$URL;}function
getLinePatterns(){return$this->_linePatterns;}function
getBlockPatterns(){return$this->_blockPatterns;}function
getDOM(){return$this->DOM;}function
tabCb($m){return$m[1].str_repeat(' ',$this->tabWidth-strlen($m[1])%$this->tabWidth);}function
free(){foreach(array_keys(get_object_vars($this))as$key)$this->$key=NULL;}function
__clone(){}}define('TEXY_CHAR','A-Za-z\x{C0}-\x{2FF}\x{370}-\x{1EFF}');define('TEXY_MARK',"\x14-\x1F");define('TEXY_MODIFIER','(?: *(?<= |^)\\.((?:\\([^)\\n]+\\)|\\[[^\\]\\n]+\\]|\\{[^}\\n]+\\}){1,3}?))');define('TEXY_MODIFIER_H','(?: *(?<= |^)\\.((?:\\([^)\\n]+\\)|\\[[^\\]\\n]+\\]|\\{[^}\\n]+\\}|<>|>|=|<){1,4}?))');define('TEXY_MODIFIER_HV','(?: *(?<= |^)\\.((?:\\([^)\\n]+\\)|\\[[^\\]\\n]+\\]|\\{[^}\\n]+\\}|<>|>|=|<|\\^|\\-|\\_){1,5}?))');define('TEXY_IMAGE','\[\*([^\n'.TEXY_MARK.']+)'.TEXY_MODIFIER.'? *(\*|>|<)\]');define('TEXY_LINK_URL','(?:\[[^\]\n]+\]|(?!\[)[^\s'.TEXY_MARK.']*?[^:);,.!?\s'.TEXY_MARK.'])');define('TEXY_LINK','(?::('.TEXY_LINK_URL.'))');define('TEXY_LINK_N','(?::('.TEXY_LINK_URL.'|:))');define('TEXY_EMAIL','[a-z0-9.+_-]{1,64}@[a-z0-9.+_-]{1,252}\.[a-z]{2,6}');define('TEXY_URLSCHEME','[a-z][a-z0-9+.-]*:');$GLOBALS['TexyHtml::$dtd']=NULL;$GLOBALS['TexyHtml::$xhtml']=TRUE;$GLOBALS['TexyHtml::$emptyElements']=array('img'=>1,'hr'=>1,'br'=>1,'input'=>1,'meta'=>1,'area'=>1,'base'=>1,'col'=>1,'link'=>1,'param'=>1,'basefont'=>1,'frame'=>1,'isindex'=>1,'wbr'=>1,'embed'=>1);$GLOBALS['TexyHtml::$inlineElements']=array('ins'=>0,'del'=>0,'tt'=>0,'i'=>0,'b'=>0,'big'=>0,'small'=>0,'em'=>0,'strong'=>0,'dfn'=>0,'code'=>0,'samp'=>0,'kbd'=>0,'var'=>0,'cite'=>0,'abbr'=>0,'acronym'=>0,'sub'=>0,'sup'=>0,'q'=>0,'span'=>0,'bdo'=>0,'a'=>0,'object'=>1,'img'=>1,'br'=>1,'script'=>1,'map'=>0,'input'=>1,'select'=>1,'textarea'=>1,'label'=>0,'button'=>1,'u'=>0,'s'=>0,'strike'=>0,'font'=>0,'applet'=>1,'basefont'=>0,'embed'=>1,'wbr'=>0,'nobr'=>0,'canvas'=>1,);$GLOBALS['TexyHtml::$optionalEnds']=array('body'=>1,'head'=>1,'html'=>1,'colgroup'=>1,'dd'=>1,'dt'=>1,'li'=>1,'option'=>1,'p'=>1,'tbody'=>1,'td'=>1,'tfoot'=>1,'th'=>1,'thead'=>1,'tr'=>1);$GLOBALS['TexyHtml::$prohibits']=array('a'=>array('a','button'),'img'=>array('pre'),'object'=>array('pre'),'big'=>array('pre'),'small'=>array('pre'),'sub'=>array('pre'),'sup'=>array('pre'),'input'=>array('button'),'select'=>array('button'),'textarea'=>array('button'),'label'=>array('button','label'),'button'=>array('button'),'form'=>array('button','form'),'fieldset'=>array('button'),'iframe'=>array('button'),'isindex'=>array('button'),);class
TexyHtml
extends
NObject4{var$name;var$isEmpty;var$attrs=array();var$children=array();var$parent;function
el($name=NULL,$attrs=NULL){$el=new
TexyHtml;$el->setName($name);if(is_array($attrs)){$el->attrs=$attrs;}elseif($attrs!==NULL){$el->setText($attrs);}return$el;}function
setName($name,$empty=NULL){if($name!==NULL&&!is_string($name)){throw(new
InvalidArgumentException('Name must be string or NULL'));}$this->name=$name;$this->isEmpty=$empty===NULL?isset($GLOBALS['TexyHtml::$emptyElements'][$name]):(bool)$empty;return$this;}function
getName(){return$this->name;}function
isEmpty(){return$this->isEmpty;}function
href($path,$query=NULL){if($query){if($query!=='')$path.='?'.$query;}$this->attrs['href']=$path;return$this;}function
setText($text){if(is_scalar($text)){$this->children=array($text);}elseif($text!==NULL){throw(new
InvalidArgumentException('Content must be scalar'));}return$this;}function
getText(){$s='';foreach($this->children
as$child){if(is_object($child))return
FALSE;$s.=$child;}return$s;}function
add($child){return$this->insert(NULL,$child);}function
create($name,$attrs=NULL){$this->insert(NULL,$child=TexyHtml::el($name,$attrs));return$child;}function
insert($index,$child,$replace=FALSE){if(is_a($child,'TexyHtml')){if($child->parent!==NULL){throw(new
InvalidStateException('Child node already has parent'));}$child->parent=$this;}elseif(!is_string($child)){throw(new
InvalidArgumentException('Child node must be scalar or TexyHtml object'));}if($index===NULL){$this->children[]=$child;}else{array_splice($this->children,(int)$index,$replace?1:0,array($child));}return$this;}function
offsetSet($index,$child){$this->insert($index,$child,TRUE);}function
offsetGet($index){return$this->children[$index];}function
offsetExists($index){return
isset($this->children[$index]);}function
offsetUnset($index){if(isset($this->children[$index])){$child=$this->children[$index];array_splice($this->children,(int)$index,1);$child->parent=NULL;}}function
count(){return
count($this->children);}function
getIterator(){return
new
ArrayIterator($this->children);}function
getChildren(){return$this->children;}function
getParent(){return$this->parent;}function
toString($texy){$ct=$this->getContentType();$s=$texy->protect($this->startTag(),$ct);if($this->isEmpty){return$s;}foreach($this->children
as$child){if(is_object($child)){$s.=$child->toString($texy);}else{$s.=$child;}}return$s.$texy->protect($this->endTag(),$ct);}function
toHtml($texy){return$texy->stringToHtml($this->toString($texy));}function
toText($texy){return$texy->stringToText($this->toString($texy));}function
startTag(){if(!$this->name){return'';}$s='<'.$this->name;if(is_array($this->attrs)){foreach($this->attrs
as$key=>$value){if($value===NULL||$value===FALSE)continue;if($value===TRUE){if($GLOBALS['TexyHtml::$xhtml'])$s.=' '.$key.'="'.$key.'"';else$s.=' '.$key;continue;}elseif(is_array($value)){$tmp=NULL;foreach($value
as$k=>$v){if($v==NULL)continue;if(is_string($k))$tmp[]=$k.':'.$v;else$tmp[]=$v;}if(!$tmp)continue;$value=implode($key==='style'?';':' ',$tmp);}$value=str_replace(array('&','"','<','>','@'),array('&amp;','&quot;','&lt;','&gt;','&#64;'),$value);$s.=' '.$key.'="'.Texy::freezeSpaces($value).'"';}}if($GLOBALS['TexyHtml::$xhtml']&&$this->isEmpty){return$s.' />';}return$s.'>';}function
endTag(){if($this->name&&!$this->isEmpty){return'</'.$this->name.'>';}return'';}function
__clone(){$this->parent=NULL;foreach($this->children
as$key=>$value){if(is_object($value)){$this->children[$key]=clone($value);}}}function
getContentType(){if(!isset($GLOBALS['TexyHtml::$inlineElements'][$this->name]))return
TEXY_CONTENT_BLOCK;return$GLOBALS['TexyHtml::$inlineElements'][$this->name]?TEXY_CONTENT_REPLACED:TEXY_CONTENT_MARKUP;}function
validateAttrs(){if(isset($GLOBALS['TexyHtml::$dtd'][$this->name])){$dtd=$GLOBALS['TexyHtml::$dtd'][$this->name][0];if(is_array($dtd)){foreach($this->attrs
as$attr=>$foo){if(!isset($dtd[$attr]))unset($this->attrs[$attr]);}}}}function
validateChild($child){if(isset($GLOBALS['TexyHtml::$dtd'][$this->name])){if(is_object($child))$child=$child->name;return
isset($GLOBALS['TexyHtml::$dtd'][$this->name][1][$child]);}else{return
TRUE;}}function
parseLine($texy,$s){$s=str_replace(array('\)','\*'),array('&#x29;','&#x2A;'),$s);$parser=new
TexyLineParser($texy,$this);$parser->parse($s);return$parser;}function
parseBlock($texy,$s,$indented=FALSE){$parser=new
TexyBlockParser($texy,$this,$indented);$parser->parse($s);}function
initDTD($strict){static$last;if($last===$strict)return;$last=$strict;$coreattrs=array('id'=>1,'class'=>1,'style'=>1,'title'=>1,'xml:id'=>1);$i18n=array('lang'=>1,'dir'=>1,'xml:lang'=>1);$attrs=$coreattrs+$i18n+array('onclick'=>1,'ondblclick'=>1,'onmousedown'=>1,'onmouseup'=>1,'onmouseover'=>1,'onmousemove'=>1,'onmouseout'=>1,'onkeypress'=>1,'onkeydown'=>1,'onkeyup'=>1);$cellalign=$attrs+array('align'=>1,'char'=>1,'charoff'=>1,'valign'=>1);$b=array('ins'=>1,'del'=>1,'p'=>1,'h1'=>1,'h2'=>1,'h3'=>1,'h4'=>1,'h5'=>1,'h6'=>1,'ul'=>1,'ol'=>1,'dl'=>1,'pre'=>1,'div'=>1,'blockquote'=>1,'noscript'=>1,'noframes'=>1,'form'=>1,'hr'=>1,'table'=>1,'address'=>1,'fieldset'=>1);if(!$strict)$b+=array('dir'=>1,'menu'=>1,'center'=>1,'iframe'=>1,'isindex'=>1,'marquee'=>1,);$i=array('ins'=>1,'del'=>1,'tt'=>1,'i'=>1,'b'=>1,'big'=>1,'small'=>1,'em'=>1,'strong'=>1,'dfn'=>1,'code'=>1,'samp'=>1,'kbd'=>1,'var'=>1,'cite'=>1,'abbr'=>1,'acronym'=>1,'sub'=>1,'sup'=>1,'q'=>1,'span'=>1,'bdo'=>1,'a'=>1,'object'=>1,'img'=>1,'br'=>1,'script'=>1,'map'=>1,'input'=>1,'select'=>1,'textarea'=>1,'label'=>1,'button'=>1,'%DATA'=>1);if(!$strict)$i+=array('u'=>1,'s'=>1,'strike'=>1,'font'=>1,'applet'=>1,'basefont'=>1,'embed'=>1,'wbr'=>1,'nobr'=>1,'canvas'=>1,);$bi=$b+$i;$GLOBALS['TexyHtml::$dtd']=array('html'=>array($strict?$i18n+array('xmlns'=>1):$i18n+array('version'=>1,'xmlns'=>1),array('head'=>1,'body'=>1),),'head'=>array($i18n+array('profile'=>1),array('title'=>1,'script'=>1,'style'=>1,'base'=>1,'meta'=>1,'link'=>1,'object'=>1,'isindex'=>1),),'title'=>array(array(),array('%DATA'=>1),),'body'=>array($attrs+array('onload'=>1,'onunload'=>1),$strict?array('script'=>1)+$b:$bi,),'script'=>array(array('charset'=>1,'type'=>1,'src'=>1,'defer'=>1,'event'=>1,'for'=>1),array('%DATA'=>1),),'style'=>array($i18n+array('type'=>1,'media'=>1,'title'=>1),array('%DATA'=>1),),'p'=>array($strict?$attrs:$attrs+array('align'=>1),$i,),'h1'=>array($strict?$attrs:$attrs+array('align'=>1),$i,),'h2'=>array($strict?$attrs:$attrs+array('align'=>1),$i,),'h3'=>array($strict?$attrs:$attrs+array('align'=>1),$i,),'h4'=>array($strict?$attrs:$attrs+array('align'=>1),$i,),'h5'=>array($strict?$attrs:$attrs+array('align'=>1),$i,),'h6'=>array($strict?$attrs:$attrs+array('align'=>1),$i,),'ul'=>array($strict?$attrs:$attrs+array('type'=>1,'compact'=>1),array('li'=>1),),'ol'=>array($strict?$attrs:$attrs+array('type'=>1,'compact'=>1,'start'=>1),array('li'=>1),),'li'=>array($strict?$attrs:$attrs+array('type'=>1,'value'=>1),$bi,),'dl'=>array($strict?$attrs:$attrs+array('compact'=>1),array('dt'=>1,'dd'=>1),),'dt'=>array($attrs,$i,),'dd'=>array($attrs,$bi,),'pre'=>array($strict?$attrs:$attrs+array('width'=>1),array_flip(array_diff(array_keys($i),array('img','object','applet','big','small','sub','sup','font','basefont'))),),'div'=>array($strict?$attrs:$attrs+array('align'=>1),$bi,),'blockquote'=>array($attrs+array('cite'=>1),$strict?array('script'=>1)+$b:$bi,),'noscript'=>array($attrs,$bi,),'form'=>array($attrs+array('action'=>1,'method'=>1,'enctype'=>1,'accept'=>1,'name'=>1,'onsubmit'=>1,'onreset'=>1,'accept-charset'=>1),$strict?array('script'=>1)+$b:$bi,),'table'=>array($attrs+array('summary'=>1,'width'=>1,'border'=>1,'frame'=>1,'rules'=>1,'cellspacing'=>1,'cellpadding'=>1,'datapagesize'=>1),array('caption'=>1,'colgroup'=>1,'col'=>1,'thead'=>1,'tbody'=>1,'tfoot'=>1,'tr'=>1),),'caption'=>array($strict?$attrs:$attrs+array('align'=>1),$i,),'colgroup'=>array($cellalign+array('span'=>1,'width'=>1),array('col'=>1),),'thead'=>array($cellalign,array('tr'=>1),),'tbody'=>array($cellalign,array('tr'=>1),),'tfoot'=>array($cellalign,array('tr'=>1),),'tr'=>array($strict?$cellalign:$cellalign+array('bgcolor'=>1),array('td'=>1,'th'=>1),),'td'=>array($cellalign+array('abbr'=>1,'axis'=>1,'headers'=>1,'scope'=>1,'rowspan'=>1,'colspan'=>1),$bi,),'th'=>array($cellalign+array('abbr'=>1,'axis'=>1,'headers'=>1,'scope'=>1,'rowspan'=>1,'colspan'=>1),$bi,),'address'=>array($attrs,$strict?$i:array('p'=>1)+$i,),'fieldset'=>array($attrs,array('legend'=>1)+$bi,),'legend'=>array($strict?$attrs+array('accesskey'=>1):$attrs+array('accesskey'=>1,'align'=>1),$i,),'tt'=>array($attrs,$i,),'i'=>array($attrs,$i,),'b'=>array($attrs,$i,),'big'=>array($attrs,$i,),'small'=>array($attrs,$i,),'em'=>array($attrs,$i,),'strong'=>array($attrs,$i,),'dfn'=>array($attrs,$i,),'code'=>array($attrs,$i,),'samp'=>array($attrs,$i,),'kbd'=>array($attrs,$i,),'var'=>array($attrs,$i,),'cite'=>array($attrs,$i,),'abbr'=>array($attrs,$i,),'acronym'=>array($attrs,$i,),'sub'=>array($attrs,$i,),'sup'=>array($attrs,$i,),'q'=>array($attrs+array('cite'=>1),$i,),'span'=>array($attrs,$i,),'bdo'=>array($coreattrs+array('lang'=>1,'dir'=>1),$i,),'a'=>array($attrs+array('charset'=>1,'type'=>1,'name'=>1,'href'=>1,'hreflang'=>1,'rel'=>1,'rev'=>1,'accesskey'=>1,'shape'=>1,'coords'=>1,'tabindex'=>1,'onfocus'=>1,'onblur'=>1),$i,),'object'=>array($attrs+array('declare'=>1,'classid'=>1,'codebase'=>1,'data'=>1,'type'=>1,'codetype'=>1,'archive'=>1,'standby'=>1,'height'=>1,'width'=>1,'usemap'=>1,'name'=>1,'tabindex'=>1),array('param'=>1)+$bi,),'map'=>array($attrs+array('name'=>1),array('area'=>1)+$b,),'select'=>array($attrs+array('name'=>1,'size'=>1,'multiple'=>1,'disabled'=>1,'tabindex'=>1,'onfocus'=>1,'onblur'=>1,'onchange'=>1),array('option'=>1,'optgroup'=>1),),'optgroup'=>array($attrs+array('disabled'=>1,'label'=>1),array('option'=>1),),'option'=>array($attrs+array('selected'=>1,'disabled'=>1,'label'=>1,'value'=>1),array('%DATA'=>1),),'textarea'=>array($attrs+array('name'=>1,'rows'=>1,'cols'=>1,'disabled'=>1,'readonly'=>1,'tabindex'=>1,'accesskey'=>1,'onfocus'=>1,'onblur'=>1,'onselect'=>1,'onchange'=>1),array('%DATA'=>1),),'label'=>array($attrs+array('for'=>1,'accesskey'=>1,'onfocus'=>1,'onblur'=>1),$i,),'button'=>array($attrs+array('name'=>1,'value'=>1,'type'=>1,'disabled'=>1,'tabindex'=>1,'accesskey'=>1,'onfocus'=>1,'onblur'=>1),$bi,),'ins'=>array($attrs+array('cite'=>1,'datetime'=>1),0,),'del'=>array($attrs+array('cite'=>1,'datetime'=>1),0,),'img'=>array($attrs+array('src'=>1,'alt'=>1,'longdesc'=>1,'name'=>1,'height'=>1,'width'=>1,'usemap'=>1,'ismap'=>1),FALSE,),'hr'=>array($strict?$attrs:$attrs+array('align'=>1,'noshade'=>1,'size'=>1,'width'=>1),FALSE,),'br'=>array($strict?$coreattrs:$coreattrs+array('clear'=>1),FALSE,),'input'=>array($attrs+array('type'=>1,'name'=>1,'value'=>1,'checked'=>1,'disabled'=>1,'readonly'=>1,'size'=>1,'maxlength'=>1,'src'=>1,'alt'=>1,'usemap'=>1,'ismap'=>1,'tabindex'=>1,'accesskey'=>1,'onfocus'=>1,'onblur'=>1,'onselect'=>1,'onchange'=>1,'accept'=>1),FALSE,),'meta'=>array($i18n+array('http-equiv'=>1,'name'=>1,'content'=>1,'scheme'=>1),FALSE,),'area'=>array($attrs+array('shape'=>1,'coords'=>1,'href'=>1,'nohref'=>1,'alt'=>1,'tabindex'=>1,'accesskey'=>1,'onfocus'=>1,'onblur'=>1),FALSE,),'base'=>array($strict?array('href'=>1):array('href'=>1,'target'=>1),FALSE,),'col'=>array($cellalign+array('span'=>1,'width'=>1),FALSE,),'link'=>array($attrs+array('charset'=>1,'href'=>1,'hreflang'=>1,'type'=>1,'rel'=>1,'rev'=>1,'media'=>1),FALSE,),'param'=>array(array('id'=>1,'name'=>1,'value'=>1,'valuetype'=>1,'type'=>1),FALSE,),'%BASE'=>array(NULL,array('html'=>1,'head'=>1,'body'=>1,'script'=>1)+$bi,),);if($strict)return;$GLOBALS['TexyHtml::$dtd']+=array('dir'=>array($attrs+array('compact'=>1),array('li'=>1),),'menu'=>array($attrs+array('compact'=>1),array('li'=>1),),'center'=>array($attrs,$bi,),'iframe'=>array($coreattrs+array('longdesc'=>1,'name'=>1,'src'=>1,'frameborder'=>1,'marginwidth'=>1,'marginheight'=>1,'scrolling'=>1,'align'=>1,'height'=>1,'width'=>1),$bi,),'noframes'=>array($attrs,$bi,),'u'=>array($attrs,$i,),'s'=>array($attrs,$i,),'strike'=>array($attrs,$i,),'font'=>array($coreattrs+$i18n+array('size'=>1,'color'=>1,'face'=>1),$i,),'applet'=>array($coreattrs+array('codebase'=>1,'archive'=>1,'code'=>1,'object'=>1,'alt'=>1,'name'=>1,'width'=>1,'height'=>1,'align'=>1,'hspace'=>1,'vspace'=>1),array('param'=>1)+$bi,),'basefont'=>array(array('id'=>1,'size'=>1,'color'=>1,'face'=>1),FALSE,),'isindex'=>array($coreattrs+$i18n+array('prompt'=>1),FALSE,),'marquee'=>array(TEXY_ALL,$bi,),'nobr'=>array(array(),$i,),'canvas'=>array(TEXY_ALL,$i,),'embed'=>array(TEXY_ALL,FALSE,),'wbr'=>array(array(),FALSE,),);$GLOBALS['TexyHtml::$dtd']['a'][0]+=array('target'=>1);$GLOBALS['TexyHtml::$dtd']['area'][0]+=array('target'=>1);$GLOBALS['TexyHtml::$dtd']['body'][0]+=array('background'=>1,'bgcolor'=>1,'text'=>1,'link'=>1,'vlink'=>1,'alink'=>1);$GLOBALS['TexyHtml::$dtd']['form'][0]+=array('target'=>1);$GLOBALS['TexyHtml::$dtd']['img'][0]+=array('align'=>1,'border'=>1,'hspace'=>1,'vspace'=>1);$GLOBALS['TexyHtml::$dtd']['input'][0]+=array('align'=>1);$GLOBALS['TexyHtml::$dtd']['link'][0]+=array('target'=>1);$GLOBALS['TexyHtml::$dtd']['object'][0]+=array('align'=>1,'border'=>1,'hspace'=>1,'vspace'=>1);$GLOBALS['TexyHtml::$dtd']['script'][0]+=array('language'=>1);$GLOBALS['TexyHtml::$dtd']['table'][0]+=array('align'=>1,'bgcolor'=>1);$GLOBALS['TexyHtml::$dtd']['td'][0]+=array('nowrap'=>1,'bgcolor'=>1,'width'=>1,'height'=>1);$GLOBALS['TexyHtml::$dtd']['th'][0]+=array('nowrap'=>1,'bgcolor'=>1,'width'=>1,'height'=>1);}}$GLOBALS['TexyModifier::$elAttrs']=array('abbr'=>1,'accesskey'=>1,'align'=>1,'alt'=>1,'archive'=>1,'axis'=>1,'bgcolor'=>1,'cellpadding'=>1,'cellspacing'=>1,'char'=>1,'charoff'=>1,'charset'=>1,'cite'=>1,'classid'=>1,'codebase'=>1,'codetype'=>1,'colspan'=>1,'compact'=>1,'coords'=>1,'data'=>1,'datetime'=>1,'declare'=>1,'dir'=>1,'face'=>1,'frame'=>1,'headers'=>1,'href'=>1,'hreflang'=>1,'hspace'=>1,'ismap'=>1,'lang'=>1,'longdesc'=>1,'name'=>1,'noshade'=>1,'nowrap'=>1,'onblur'=>1,'onclick'=>1,'ondblclick'=>1,'onkeydown'=>1,'onkeypress'=>1,'onkeyup'=>1,'onmousedown'=>1,'onmousemove'=>1,'onmouseout'=>1,'onmouseover'=>1,'onmouseup'=>1,'rel'=>1,'rev'=>1,'rowspan'=>1,'rules'=>1,'scope'=>1,'shape'=>1,'size'=>1,'span'=>1,'src'=>1,'standby'=>1,'start'=>1,'summary'=>1,'tabindex'=>1,'target'=>1,'title'=>1,'type'=>1,'usemap'=>1,'valign'=>1,'value'=>1,'vspace'=>1,);class
TexyModifier
extends
NObject4{var$id;var$classes=array();var$styles=array();var$attrs=array();var$hAlign;var$vAlign;var$title;var$cite;function
__construct($mod=NULL){$this->setProperties($mod);}function
setProperties($mod){if(!$mod)return;$p=0;$len=strlen($mod);while($p<$len){$ch=$mod[$p];if($ch==='('){$a=strpos($mod,')',$p)+1;$this->title=Texy::unescapeHtml(trim(substr($mod,$p+1,$a-$p-2)));$p=$a;}elseif($ch==='{'){$a=strpos($mod,'}',$p)+1;foreach(explode(';',substr($mod,$p+1,$a-$p-2))as$value){$pair=explode(':',$value,2);$prop=strtolower(trim($pair[0]));if($prop===''||!isset($pair[1]))continue;$value=trim($pair[1]);if(isset($GLOBALS['TexyModifier::$elAttrs'][$prop]))$this->attrs[$prop]=$value;elseif($value!=='')$this->styles[$prop]=$value;}$p=$a;}elseif($ch==='['){$a=strpos($mod,']',$p)+1;$s=str_replace('#',' #',substr($mod,$p+1,$a-$p-2));foreach(explode(' ',$s)as$value){if($value==='')continue;if($value{0}==='#')$this->id=substr($value,1);else$this->classes[$value]=TRUE;}$p=$a;}elseif($ch==='^'){$this->vAlign='top';$p++;}elseif($ch==='-'){$this->vAlign='middle';$p++;}elseif($ch==='_'){$this->vAlign='bottom';$p++;}elseif($ch==='='){$this->hAlign='justify';$p++;}elseif($ch==='>'){$this->hAlign='right';$p++;}elseif(substr($mod,$p,2)==='<>'){$this->hAlign='center';$p+=2;}elseif($ch==='<'){$this->hAlign='left';$p++;}else{break;}}}function
decorate($texy,$el){$elAttrs=&$el->attrs;$tmp=$texy->allowedTags;if(!$this->attrs){}elseif($tmp===TEXY_ALL){$elAttrs=$this->attrs;$el->validateAttrs();}elseif(is_array($tmp)&&isset($tmp[$el->getName()])){$tmp=$tmp[$el->getName()];if($tmp===TEXY_ALL){$elAttrs=$this->attrs;}elseif(is_array($tmp)&&count($tmp)){$tmp=array_flip($tmp);foreach($this->attrs
as$key=>$value)if(isset($tmp[$key]))$el->attrs[$key]=$value;}$el->validateAttrs();}if($this->title!==NULL)$elAttrs['title']=$texy->typographyModule->postLine($this->title);if($this->classes||$this->id!==NULL){$tmp=$texy->_classes;if($tmp===TEXY_ALL){foreach($this->classes
as$value=>$foo)$elAttrs['class'][]=$value;$elAttrs['id']=$this->id;}elseif(is_array($tmp)){foreach($this->classes
as$value=>$foo)if(isset($tmp[$value]))$elAttrs['class'][]=$value;if(isset($tmp['#'.$this->id]))$elAttrs['id']=$this->id;}}if($this->styles){$tmp=$texy->_styles;if($tmp===TEXY_ALL){foreach($this->styles
as$prop=>$value)$elAttrs['style'][$prop]=$value;}elseif(is_array($tmp)){foreach($this->styles
as$prop=>$value)if(isset($tmp[$prop]))$elAttrs['style'][$prop]=$value;}}if($this->hAlign){if(empty($texy->alignClasses[$this->hAlign])){$elAttrs['style']['text-align']=$this->hAlign;}else{$elAttrs['class'][]=$texy->alignClasses[$this->hAlign];}}if($this->vAlign){if(empty($texy->alignClasses[$this->vAlign])){$elAttrs['style']['vertical-align']=$this->vAlign;}else{$elAttrs['class'][]=$texy->alignClasses[$this->vAlign];}}return$el;}}class
TexyModule
extends
NObject4{var$texy;}class
TexyParser
extends
NObject4{var$texy;var$element;var$patterns;function
getTexy(){return$this->texy;}}class
TexyBlockParser
extends
TexyParser{var$text;var$offset;var$indented;function
__construct($texy,$element,$indented){$this->texy=$texy;$this->element=$element;$this->indented=(bool)$indented;$this->patterns=$texy->getBlockPatterns();}function
isIndented(){return$this->indented;}function
next($pattern,&$matches){$matches=NULL;$ok=preg_match($pattern.'Am',$this->text,$matches,PREG_OFFSET_CAPTURE,$this->offset);if($ok){$this->offset+=strlen($matches[0][0])+1;foreach($matches
as$key=>$value)$matches[$key]=$value[0];}return$ok;}function
moveBackward($linesCount=1){while(--$this->offset>0)if($this->text{$this->offset-1}==="\n"){$linesCount--;if($linesCount<1)break;}$this->offset=max($this->offset,0);}function
cmp($a,$b){if($a[0]===$b[0])return$a[3]<$b[3]?-1:1;if($a[0]<$b[0])return-1;return
1;}function
parse($text){$tx=$this->texy;$tx->invokeHandlers('beforeBlockParse',array($this,&$text));$this->text=$text;$this->offset=0;$matches=array();$priority=0;foreach($this->patterns
as$name=>$pattern){preg_match_all($pattern['pattern'],$text,$ms,PREG_OFFSET_CAPTURE|PREG_SET_ORDER);foreach($ms
as$m){$offset=$m[0][1];foreach($m
as$k=>$v)$m[$k]=$v[0];$matches[]=array($offset,$name,$m,$priority);}$priority++;}unset($name,$pattern,$ms,$m,$k,$v);usort($matches,array(__CLASS__,'cmp'));$matches[]=array(strlen($text),NULL,NULL);$el=$this->element;$cursor=0;do{do{list($mOffset,$mName,$mMatches)=$matches[$cursor];$cursor++;if($mName===NULL)break;if($mOffset>=$this->offset)break;}while(1);if($mOffset>$this->offset){$s=trim(substr($text,$this->offset,$mOffset-$this->offset));if($s!==''){$tx->paragraphModule->process($this,$s,$el);}}if($mName===NULL)break;$this->offset=$mOffset+strlen($mMatches[0])+1;$res=call_user_func_array($this->patterns[$mName]['handler'],array($this,$mMatches,$mName));if($res===FALSE||$this->offset<=$mOffset){$this->offset=$mOffset;continue;}elseif(is_a($res,'TexyHtml')){$el->insert(NULL,$res);}elseif(is_string($res)){$el->insert(NULL,$res);}}while(1);}}class
TexyLineParser
extends
TexyParser{var$again;function
__construct($texy,$element){$this->texy=$texy;$this->element=$element;$this->patterns=$texy->getLinePatterns();}function
parse($text){$tx=$this->texy;$pl=$this->patterns;if(!$pl){$this->element->insert(NULL,$text);return;}$offset=0;$names=array_keys($pl);$arrMatches=$arrOffset=array();foreach($names
as$name)$arrOffset[$name]=-1;do{$min=NULL;$minOffset=strlen($text);foreach($names
as$index=>$name){if($arrOffset[$name]<$offset){$delta=($arrOffset[$name]===-2)?1:0;if(preg_match($pl[$name]['pattern'],$text,$arrMatches[$name],PREG_OFFSET_CAPTURE,$offset+$delta)){$m=&$arrMatches[$name];if(!strlen($m[0][0]))continue;$arrOffset[$name]=$m[0][1];foreach($m
as$keyx=>$value)$m[$keyx]=$value[0];}else{continue;}}if($arrOffset[$name]<$minOffset){$minOffset=$arrOffset[$name];$min=$name;}}if($min===NULL)break;$px=$pl[$min];$offset=$start=$arrOffset[$min];$this->again=FALSE;$res=call_user_func_array($px['handler'],array($this,$arrMatches[$min],$min));if(is_a($res,'TexyHtml')){$res=$res->toString($tx);}elseif($res===FALSE){$arrOffset[$min]=-2;continue;}$len=strlen($arrMatches[$min][0]);$text=substr_replace($text,(string)$res,$start,$len);$delta=strlen($res)-$len;foreach($names
as$name){if($arrOffset[$name]<$start+$len)$arrOffset[$name]=-1;else$arrOffset[$name]+=$delta;}if($this->again){$arrOffset[$min]=-2;}else{$arrOffset[$min]=-1;$offset+=strlen($res);}}while(1);$this->element->insert(NULL,$text);}}$GLOBALS['TexyUtf::$xlat']=NULL;$GLOBALS['TexyUtf::$xlatCache']=NULL;class
TexyUtf{function
__construct(){throw(new
LogicException("Cannot instantiate static class ".get_class($this)));}function
toUtf($s,$encoding){if(strcasecmp($encoding,'utf-8')===0)return$s;return
iconv($encoding,'UTF-8',$s);}function
utfTo($s,$encoding){if(strcasecmp($encoding,'utf-8')===0)return$s;return
iconv('utf-8',$encoding.'//TRANSLIT',$s);}function
strtolower($s){if(function_exists('mb_strtolower'))return
mb_strtolower($s,'UTF-8');return
strtr(iconv('UTF-8','WINDOWS-1250//IGNORE',$s),"ABCDEFGHIJKLMNOPQRSTUVWXYZ\x8a\x8c\x8d\x8e\x8f\xa3\xa5\xaa\xaf\xbc\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd8\xd9\xda\xdb\xdc\xdd\xde","abcdefghijklmnopqrstuvwxyz\x9a\x9c\x9d\x9e\x9f\xb3\xb9\xba\xbf\xbe\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf8\xf9\xfa\xfb\xfc\xfd\xfe");}function
utf2ascii($s){$s=iconv('UTF-8','WINDOWS-1250//IGNORE',$s);return
strtr($s,"\xa5\xa3\xbc\x8c\xa7\x8a\xaa\x8d\x8f\x8e\xaf\xb9\xb3\xbe\x9c\x9a\xba\x9d\x9f\x9e\xbf\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf8\xf9\xfa\xfb\xfc\xfd\xfe","ALLSSSSTZZZallssstzzzRAAAALCCCEEEEIIDDNNOOOOxRUUUUYTsraaaalccceeeeiiddnnooooruuuuyt");}function
utf2html($s,$encoding){if(strcasecmp($encoding,'utf-8')===0)return$s;$GLOBALS['TexyUtf::$xlat']=&$GLOBALS['TexyUtf::$xlatCache'][strtolower($encoding)];if(!$GLOBALS['TexyUtf::$xlat']){for($i=128;$i<256;$i++){$ch=@iconv($encoding,'UTF-8//IGNORE',chr($i));if($ch)$GLOBALS['TexyUtf::$xlat'][$ch]=chr($i);}}return
preg_replace_callback('#[\x80-\x{FFFF}]#u',array(__CLASS__,'cb'),$s);}function
cb($m){$m=$m[0];if(isset($GLOBALS['TexyUtf::$xlat'][$m]))return$GLOBALS['TexyUtf::$xlat'][$m];$ch1=ord($m[0]);$ch2=ord($m[1]);if(($ch2>>6)!==2)return'';if(($ch1&0xE0)===0xC0)return'&#'.((($ch1&0x1F)<<6)+($ch2&0x3F)).';';if(($ch1&0xF0)===0xE0){$ch3=ord($m[2]);if(($ch3>>6)!==2)return'';return'&#'.((($ch1&0xF)<<12)+(($ch2&0x3F)<<06)+(($ch3&0x3F))).';';}return'';}}$GLOBALS['TexyConfigurator::$safeTags']=array('a'=>array('href','title'),'acronym'=>array('title'),'b'=>array(),'br'=>array(),'cite'=>array(),'code'=>array(),'em'=>array(),'i'=>array(),'strong'=>array(),'sub'=>array(),'sup'=>array(),'q'=>array(),'small'=>array(),);class
TexyConfigurator{function
__construct(){throw(new
LogicException("Cannot instantiate static class ".get_class($this)));}function
safeMode($texy){$texy->allowedClasses=TEXY_NONE;$texy->allowedStyles=TEXY_NONE;$texy->allowedTags=$GLOBALS['TexyConfigurator::$safeTags'];$texy->urlSchemeFilters['a']='#https?:|ftp:|mailto:#A';$texy->urlSchemeFilters['i']='#https?:#A';$texy->urlSchemeFilters['c']='#http:#A';$texy->allowed['image']=FALSE;$texy->allowed['link/definition']=FALSE;$texy->allowed['html/comment']=FALSE;$texy->linkModule->forceNoFollow=TRUE;}function
disableLinks($texy){$texy->allowed['link/reference']=FALSE;$texy->allowed['link/email']=FALSE;$texy->allowed['link/url']=FALSE;$texy->allowed['link/definition']=FALSE;$texy->phraseModule->linksAllowed=FALSE;if(is_array($texy->allowedTags)){unset($texy->allowedTags['a']);}}function
disableImages($texy){$texy->allowed['image']=FALSE;$texy->allowed['figure']=FALSE;$texy->allowed['image/definition']=FALSE;if(is_array($texy->allowedTags)){unset($texy->allowedTags['img'],$texy->allowedTags['object'],$texy->allowedTags['embed'],$texy->allowedTags['applet']);}}}class
TexyHandlerInvocation
extends
NObject4{var$handlers;var$pos;var$args;var$parser;function
__construct($handlers,$parser,$args){$this->handlers=$handlers;$this->pos=count($handlers);$this->parser=$parser;array_unshift($args,$this);$this->args=$args;}function
proceed(){if($this->pos===0){throw(new
InvalidStateException('No more handlers'));}if(func_num_args()){$this->args=func_get_args();array_unshift($this->args,$this);}$this->pos--;$res=call_user_func_array($this->handlers[$this->pos],$this->args);if($res===NULL){throw(new
UnexpectedValueException("Invalid value returned from handler '".print_r($this->handlers[$this->pos],TRUE)."'"));}return$res;}function
getParser(){return$this->parser;}function
getTexy(){return$this->parser->getTexy();}function
free(){$this->handlers=$this->parser=$this->args=NULL;}}class
TexyParagraphModule
extends
TexyModule{function
__construct($texy){$this->texy=$texy;$texy->addHandler('paragraph',array($this,'solve'));}function
process($parser,$content,$el){$tx=$this->texy;if($parser->isIndented()){$parts=preg_split('#(\n(?! )|\n{2,})#',$content,-1,PREG_SPLIT_NO_EMPTY);}else{$parts=preg_split('#(\n{2,})#',$content,-1,PREG_SPLIT_NO_EMPTY);}foreach($parts
as$s){$s=trim($s);if($s==='')continue;$mx=$mod=NULL;if(preg_match('#\A(.*)(?<=\A|\S)'.TEXY_MODIFIER_H.'(\n.*)?()\z#sUm',$s,$mx)){list(,$mC1,$mMod,$mC2)=$mx;$s=trim($mC1.$mC2);if($s==='')continue;$mod=new
TexyModifier;$mod->setProperties($mMod);}$res=$tx->invokeAroundHandlers('paragraph',$parser,array($s,$mod));if($res)$el->insert(NULL,$res);}}function
solve($invocation,$content,$mod){$tx=$this->texy;if($tx->mergeLines){$content=preg_replace('#\n +(?=\S)#',"\r",$content);}else{$content=preg_replace('#\n#',"\r",$content);}$el=TexyHtml::el('p');$el->parseLine($tx,$content);$content=$el->getText();if(strpos($content,TEXY_CONTENT_BLOCK)!==FALSE){$el->setName(NULL);}elseif(strpos($content,TEXY_CONTENT_TEXTUAL)!==FALSE){}elseif(preg_match('#[^\s'.TEXY_MARK.']#u',$content)){}elseif(strpos($content,TEXY_CONTENT_REPLACED)!==FALSE){$el->setName($tx->nontextParagraph);}else{if(!$mod)$el->setName(NULL);}if($el->getName()){if($mod)$mod->decorate($tx,$el);if(strpos($content,"\r")!==FALSE){$key=$tx->protect('<br />',TEXY_CONTENT_REPLACED);$content=str_replace("\r",$key,$content);};}$content=strtr($content,"\r\n",'  ');$el->setText($content);return$el;}}class
TexyBlockModule
extends
TexyModule{function
__construct($texy){$this->texy=$texy;$texy->allowed['block/default']=TRUE;$texy->allowed['block/pre']=TRUE;$texy->allowed['block/code']=TRUE;$texy->allowed['block/html']=TRUE;$texy->allowed['block/text']=TRUE;$texy->allowed['block/texysource']=TRUE;$texy->allowed['block/comment']=TRUE;$texy->allowed['block/div']=TRUE;$texy->addHandler('block',array($this,'solve'));$texy->addHandler('beforeBlockParse',array($this,'beforeBlockParse'));$texy->registerBlockPattern(array($this,'pattern'),'#^/--++ *+(.*)'.TEXY_MODIFIER_H.'?$((?:\n(?0)|\n.*+)*)(?:\n\\\\--.*$|\z)#mUi','blocks');}function
beforeBlockParse($parser,&$text){$text=preg_replace('#^(/--++ *+(?!div|texysource).*)$((?:\n.*+)*?)(?:\n\\\\--.*$|(?=(\n/--.*$)))#mi',"\$1\$2\n\\--",$text);}function
pattern($parser,$matches){list(,$mParam,$mMod,$mContent)=$matches;$mod=new
TexyModifier($mMod);$parts=preg_split('#\s+#u',$mParam,2);$blocktype=empty($parts[0])?'block/default':'block/'.$parts[0];$param=empty($parts[1])?NULL:$parts[1];return$this->texy->invokeAroundHandlers('block',$parser,array($blocktype,$mContent,$param,$mod));}function
outdent($s){trigger_error('Use Texy::outdent()',E_USER_WARNING);return
Texy::outdent($s);}function
solve($invocation,$blocktype,$s,$param,$mod){$tx=$this->texy;$parser=$invocation->getParser();if($blocktype==='block/texy'){$el=TexyHtml::el();$el->parseBlock($tx,$s,$parser->isIndented());return$el;}if(empty($tx->allowed[$blocktype]))return
FALSE;if($blocktype==='block/texysource'){$s=Texy::outdent($s);if($s==='')return"\n";$el=TexyHtml::el();if($param==='line')$el->parseLine($tx,$s);else$el->parseBlock($tx,$s);$s=$el->toHtml($tx);$blocktype='block/code';$param='html';}if($blocktype==='block/code'){$s=Texy::outdent($s);if($s==='')return"\n";$s=Texy::escapeHtml($s);$s=$tx->protect($s,TEXY_CONTENT_BLOCK);$el=TexyHtml::el('pre');$mod->decorate($tx,$el);$el->attrs['class'][]=$param;$el->create('code',$s);return$el;}if($blocktype==='block/default'){$s=Texy::outdent($s);if($s==='')return"\n";$el=TexyHtml::el('pre');$mod->decorate($tx,$el);$el->attrs['class'][]=$param;$s=Texy::escapeHtml($s);$s=$tx->protect($s,TEXY_CONTENT_BLOCK);$el->setText($s);return$el;}if($blocktype==='block/pre'){$s=Texy::outdent($s);if($s==='')return"\n";$el=TexyHtml::el('pre');$mod->decorate($tx,$el);$lineParser=new
TexyLineParser($tx,$el);$tmp=$lineParser->patterns;$lineParser->patterns=array();if(isset($tmp['html/tag']))$lineParser->patterns['html/tag']=$tmp['html/tag'];if(isset($tmp['html/comment']))$lineParser->patterns['html/comment']=$tmp['html/comment'];unset($tmp);$lineParser->parse($s);$s=$el->getText();$s=Texy::unescapeHtml($s);$s=Texy::escapeHtml($s);$s=$tx->unprotect($s);$s=$tx->protect($s,TEXY_CONTENT_BLOCK);$el->setText($s);return$el;}if($blocktype==='block/html'){$s=trim($s,"\n");if($s==='')return"\n";$el=TexyHtml::el();$lineParser=new
TexyLineParser($tx,$el);$tmp=$lineParser->patterns;$lineParser->patterns=array();if(isset($tmp['html/tag']))$lineParser->patterns['html/tag']=$tmp['html/tag'];if(isset($tmp['html/comment']))$lineParser->patterns['html/comment']=$tmp['html/comment'];unset($tmp);$lineParser->parse($s);$s=$el->getText();$s=Texy::unescapeHtml($s);$s=Texy::escapeHtml($s);$s=$tx->unprotect($s);return$tx->protect($s,TEXY_CONTENT_BLOCK)."\n";}if($blocktype==='block/text'){$s=trim($s,"\n");if($s==='')return"\n";$s=Texy::escapeHtml($s);$tmp=TexyHtml::el('br');$s=str_replace("\n",$tmp->startTag(),$s);return$tx->protect($s,TEXY_CONTENT_BLOCK)."\n";}if($blocktype==='block/comment'){return"\n";}if($blocktype==='block/div'){$s=Texy::outdent($s);if($s==='')return"\n";$el=TexyHtml::el('div');$mod->decorate($tx,$el);$el->parseBlock($tx,$s,$parser->isIndented());return$el;}return
FALSE;}}define('TEXY_HEADING_DYNAMIC',1);define('TEXY_HEADING_FIXED',2);class
TexyHeadingModule
extends
TexyModule{var$title;var$TOC;var$generateID=FALSE;var$idPrefix='toc-';var$top=1;var$moreMeansHigher=TRUE;var$balancing=TEXY_HEADING_DYNAMIC;var$levels=array('#'=>0,'*'=>1,'='=>2,'-'=>3,);var$usedID;function
__construct($texy){$this->texy=$texy;$texy->addHandler('heading',array($this,'solve'));$texy->addHandler('beforeParse',array($this,'beforeParse'));$texy->addHandler('afterParse',array($this,'afterParse'));$texy->registerBlockPattern(array($this,'patternUnderline'),'#^(\S.*)'.TEXY_MODIFIER_H.'?\n'.'(\#{3,}|\*{3,}|={3,}|-{3,})$#mU','heading/underlined');$texy->registerBlockPattern(array($this,'patternSurround'),'#^(\#{2,}+|={2,}+)(.+)'.TEXY_MODIFIER_H.'?()$#mU','heading/surrounded');}function
beforeParse(){$this->title=NULL;$this->usedID=array();$this->TOC=array();}function
afterParse($texy,$DOM,$isSingleLine){if($isSingleLine||$this->balancing===TEXY_HEADING_FIXED)return;$top=$this->top;$map=array();$min=100;foreach($this->TOC
as$item){$level=$item['level'];if($item['surrounded']){$min=min($level,$min);$top=$this->top-$min;}else{$map[$level]=$level;}}asort($map);$map=array_flip(array_values($map));foreach($this->TOC
as$key=>$item){$level=$item['level'];$level=$item['surrounded']?$level+$top:$map[$level]+$this->top;$item['el']->setName('h'.min(6,max(1,$level)));$this->TOC[$key]['level']=$level;}}function
patternUnderline($parser,$matches){list(,$mContent,$mMod,$mLine)=$matches;$mod=new
TexyModifier($mMod);$level=$this->levels[$mLine[0]];return$this->texy->invokeAroundHandlers('heading',$parser,array($level,$mContent,$mod,FALSE));}function
patternSurround($parser,$matches){list(,$mLine,$mContent,$mMod)=$matches;$mod=new
TexyModifier($mMod);$level=min(7,max(2,strlen($mLine)));$level=$this->moreMeansHigher?7-$level:$level-2;$mContent=rtrim($mContent,$mLine[0].' ');return$this->texy->invokeAroundHandlers('heading',$parser,array($level,$mContent,$mod,TRUE));}function
solve($invocation,$level,$content,$mod,$isSurrounded){$tx=$this->texy;$el=TexyHtml::el('h'.min(6,max(1,$level+$this->top)));$mod->decorate($tx,$el);$el->parseLine($tx,trim($content));$title=NULL;if($this->generateID&&empty($el->attrs['id'])){$title=trim($el->toText($tx));$id=$this->idPrefix.Texy::webalize($title);$counter='';if(isset($this->usedID[$id.$counter])){$counter=2;while(isset($this->usedID[$id.'-'.$counter]))$counter++;$id.='-'.$counter;}$this->usedID[$id]=TRUE;$el->attrs['id']=$id;}if($this->title===NULL){if($title===NULL)$title=trim($el->toText($tx));$this->title=$title;}$this->TOC[]=array('el'=>$el,'level'=>$level,'title'=>$title,'surrounded'=>$isSurrounded,);return$el;}}class
TexyHorizLineModule
extends
TexyModule{var$classes=array('-'=>NULL,'*'=>NULL,);function
__construct($texy){$this->texy=$texy;$texy->addHandler('horizline',array($this,'solve'));$texy->registerBlockPattern(array($this,'pattern'),'#^(\*{3,}|-{3,})\ *'.TEXY_MODIFIER.'?()$#mU','horizline');}function
pattern($parser,$matches){list(,$mType,$mMod)=$matches;$mod=new
TexyModifier($mMod);return$this->texy->invokeAroundHandlers('horizline',$parser,array($mType,$mod));}function
solve($invocation,$type,$modifier){$el=TexyHtml::el('hr');$modifier->decorate($invocation->getTexy(),$el);$class=$this->classes[$type[0]];if($class&&!isset($modifier->classes[$class])){$el->attrs['class'][]=$class;}return$el;}}class
TexyHtmlModule
extends
TexyModule{var$passComment=TRUE;function
__construct($texy){$this->texy=$texy;$texy->addHandler('htmlComment',array($this,'solveComment'));$texy->addHandler('htmlTag',array($this,'solveTag'));$texy->registerLinePattern(array($this,'patternTag'),'#<(/?)([a-z][a-z0-9_:-]*)((?:\s+[a-z0-9:-]+|=\s*"[^"'.TEXY_MARK.']*"|=\s*\'[^\''.TEXY_MARK.']*\'|=[^\s>'.TEXY_MARK.']+)*)\s*(/?)>#isu','html/tag');$texy->registerLinePattern(array($this,'patternComment'),'#<!--([^'.TEXY_MARK.']*?)-->#is','html/comment');}function
patternComment($parser,$matches){list(,$mComment)=$matches;return$this->texy->invokeAroundHandlers('htmlComment',$parser,array($mComment));}function
patternTag($parser,$matches){list(,$mEnd,$mTag,$mAttr,$mEmpty)=$matches;$tx=$this->texy;$isStart=$mEnd!=='/';$isEmpty=$mEmpty==='/';if(!$isEmpty&&substr($mAttr,-1)==='/'){$mAttr=substr($mAttr,0,-1);$isEmpty=TRUE;}if($isEmpty&&!$isStart)return
FALSE;$mAttr=trim(strtr($mAttr,"\n",' '));if($mAttr&&!$isStart)return
FALSE;$el=TexyHtml::el($mTag);if($isStart){$matches2=NULL;preg_match_all('#([a-z0-9:-]+)\s*(?:=\s*(\'[^\']*\'|"[^"]*"|[^\'"\s]+))?()#isu',$mAttr,$matches2,PREG_SET_ORDER);foreach($matches2
as$m){$key=strtolower($m[1]);$value=$m[2];if($value==NULL)$el->attrs[$key]=TRUE;elseif($value{0}==='\''||$value{0}==='"')$el->attrs[$key]=Texy::unescapeHtml(substr($value,1,-1));else$el->attrs[$key]=Texy::unescapeHtml($value);}}$res=$tx->invokeAroundHandlers('htmlTag',$parser,array($el,$isStart,$isEmpty));if(is_a($res,'TexyHtml')){return$tx->protect($isStart?$res->startTag():$res->endTag(),$res->getContentType());}return$res;}function
solveTag($invocation,$el,$isStart,$forceEmpty=NULL){$tx=$this->texy;$allowedTags=$tx->allowedTags;if(!$allowedTags)return
FALSE;$name=$el->getName();$lower=strtolower($name);if(isset($GLOBALS['TexyHtml::$dtd'][$lower])||$name===strtoupper($name)){$name=$lower;$el->setName($name);}if(is_array($allowedTags)){if(!isset($allowedTags[$name]))return
FALSE;$allowedAttrs=$allowedTags[$name];}else{if($forceEmpty)$el->setName($name,TRUE);$allowedAttrs=TEXY_ALL;}if(!$isStart){return$el;}$elAttrs=&$el->attrs;if(!$allowedAttrs){$elAttrs=array();}elseif(is_array($allowedAttrs)){$allowedAttrs=array_flip($allowedAttrs);foreach($elAttrs
as$key=>$foo)if(!isset($allowedAttrs[$key]))unset($elAttrs[$key]);}$tmp=$tx->_classes;if(isset($elAttrs['class'])){if(is_array($tmp)){$elAttrs['class']=explode(' ',$elAttrs['class']);foreach($elAttrs['class']as$key=>$value)if(!isset($tmp[$value]))unset($elAttrs['class'][$key]);}elseif($tmp!==TEXY_ALL){$elAttrs['class']=NULL;}}if(isset($elAttrs['id'])){if(is_array($tmp)){if(!isset($tmp['#'.$elAttrs['id']]))$elAttrs['id']=NULL;}elseif($tmp!==TEXY_ALL){$elAttrs['id']=NULL;}}if(isset($elAttrs['style'])){$tmp=$tx->_styles;if(is_array($tmp)){$styles=explode(';',$elAttrs['style']);$elAttrs['style']=NULL;foreach($styles
as$value){$pair=explode(':',$value,2);$prop=trim($pair[0]);if(isset($pair[1])&&isset($tmp[strtolower($prop)]))$elAttrs['style'][$prop]=$pair[1];}}elseif($tmp!==TEXY_ALL){$elAttrs['style']=NULL;}}if($name==='img'){if(!isset($elAttrs['src']))return
FALSE;if(!$tx->checkURL($elAttrs['src'],'i'))return
FALSE;$tx->summary['images'][]=$elAttrs['src'];}elseif($name==='a'){if(!isset($elAttrs['href'])&&!isset($elAttrs['name'])&&!isset($elAttrs['id']))return
FALSE;if(isset($elAttrs['href'])){if($tx->linkModule->forceNoFollow&&strpos($elAttrs['href'],'//')!==FALSE){if(isset($elAttrs['rel']))$elAttrs['rel']=(array)$elAttrs['rel'];$elAttrs['rel'][]='nofollow';}if(!$tx->checkURL($elAttrs['href'],'a'))return
FALSE;$tx->summary['links'][]=$elAttrs['href'];}}$el->validateAttrs();return$el;}function
solveComment($invocation,$content){if(!$this->passComment)return'';$content=preg_replace('#-{2,}#','-',$content);$content=trim($content,'-');return$this->texy->protect('<!--'.$content.'-->',TEXY_CONTENT_MARKUP);}}class
TexyFigureModule
extends
TexyModule{var$class='figure';var$leftClass;var$rightClass;var$widthDelta=10;function
__construct($texy){$this->texy=$texy;$texy->addHandler('figure',array($this,'solve'));$texy->registerBlockPattern(array($this,'pattern'),'#^'.TEXY_IMAGE.TEXY_LINK_N.'?? +\*\*\* +(.*)'.TEXY_MODIFIER_H.'?()$#mUu','figure');}function
pattern($parser,$matches){list(,$mURLs,$mImgMod,$mAlign,$mLink,$mContent,$mMod)=$matches;$tx=$this->texy;$image=$tx->imageModule->factoryImage($mURLs,$mImgMod.$mAlign);$mod=new
TexyModifier($mMod);$mContent=ltrim($mContent);if($mLink){if($mLink===':'){$link=new
TexyLink($image->linkedURL===NULL?$image->URL:$image->linkedURL);$link->raw=':';$link->type=TEXY_LINK_IMAGE;}else{$link=$tx->linkModule->factoryLink($mLink,NULL,NULL);}}else$link=NULL;return$tx->invokeAroundHandlers('figure',$parser,array($image,$link,$mContent,$mod));}function
solve($invocation,$image,$link,$content,$mod){$tx=$this->texy;$hAlign=$image->modifier->hAlign;$image->modifier->hAlign=NULL;$elImg=$tx->imageModule->solve(NULL,$image,$link);if(!$elImg)return
FALSE;$el=TexyHtml::el('div');if(!empty($image->width)&&$this->widthDelta!==FALSE){$el->attrs['style']['width']=($image->width+$this->widthDelta).'px';}$mod->decorate($tx,$el);$el->offsetSet(0,$elImg);$el->offsetSet(1,$elP=TexyHtml::el('p'));$elP->parseLine($tx,ltrim($content));$class=$this->class;if($hAlign){$var=$hAlign.'Class';if(!empty($this->$var)){$class=$this->$var;}elseif(empty($tx->alignClasses[$hAlign])){$el->attrs['style']['float']=$hAlign;}else{$class.='-'.$tx->alignClasses[$hAlign];}}$el->attrs['class'][]=$class;return$el;}}class
TexyImageModule
extends
TexyModule{var$root='images/';var$linkedRoot='images/';var$fileRoot=NULL;var$leftClass;var$rightClass;var$defaultAlt='';var$onLoad="var i=new Image();i.src='%i';if(typeof preload=='undefined')preload=new Array();preload[preload.length]=i;this.onload=''";var$references=array();function
__construct($texy){$this->texy=$texy;$texy->allowed['image/definition']=TRUE;$texy->addHandler('image',array($this,'solve'));$texy->addHandler('beforeParse',array($this,'beforeParse'));$texy->registerLinePattern(array($this,'patternImage'),'#'.TEXY_IMAGE.TEXY_LINK_N.'??()#Uu','image');}function
beforeParse($texy,&$text){if($texy->allowed['image/definition']){$text=preg_replace_callback('#^\[\*([^\n]+)\*\]:\ +(.+)\ *'.TEXY_MODIFIER.'?\s*()$#mUu',array($this,'patternReferenceDef'),$text);}}function
patternReferenceDef($matches){list(,$mRef,$mURLs,$mMod)=$matches;$image=$this->factoryImage($mURLs,$mMod,FALSE);$this->addReference($mRef,$image);return'';}function
patternImage($parser,$matches){list(,$mURLs,$mMod,$mAlign,$mLink)=$matches;$tx=$this->texy;$image=$this->factoryImage($mURLs,$mMod.$mAlign);if($mLink){if($mLink===':'){$link=new
TexyLink($image->linkedURL===NULL?$image->URL:$image->linkedURL);$link->raw=':';$link->type=TEXY_LINK_IMAGE;}else{$link=$tx->linkModule->factoryLink($mLink,NULL,NULL);}}else$link=NULL;return$tx->invokeAroundHandlers('image',$parser,array($image,$link));}function
addReference($name,$image){$image->name=TexyUtf::strtolower($name);$this->references[$image->name]=$image;}function
getReference($name){$name=TexyUtf::strtolower($name);if(isset($this->references[$name]))return
clone($this->references[$name]);return
FALSE;}function
factoryImage($content,$mod,$tryRef=TRUE){$image=$tryRef?$this->getReference(trim($content)):FALSE;if(!$image){$tx=$this->texy;$content=explode('|',$content);$image=new
TexyImage;$matches=NULL;if(preg_match('#^(.*) (\d+|\?) *(X|x) *(\d+|\?) *()$#U',$content[0],$matches)){$image->URL=trim($matches[1]);$image->asMax=$matches[3]==='X';$image->width=$matches[2]==='?'?NULL:(int)$matches[2];$image->height=$matches[4]==='?'?NULL:(int)$matches[4];}else{$image->URL=trim($content[0]);}if(!$tx->checkURL($image->URL,'i'))$image->URL=NULL;if(isset($content[1])){$tmp=trim($content[1]);if($tmp!==''&&$tx->checkURL($tmp,'i'))$image->overURL=$tmp;}if(isset($content[2])){$tmp=trim($content[2]);if($tmp!==''&&$tx->checkURL($tmp,'a'))$image->linkedURL=$tmp;}}$image->modifier->setProperties($mod);return$image;}function
solve($invocation,$image,$link){if($image->URL==NULL)return
FALSE;$tx=$this->texy;$mod=$image->modifier;$alt=$mod->title;$mod->title=NULL;$hAlign=$mod->hAlign;$mod->hAlign=NULL;$el=TexyHtml::el('img');$el->attrs['src']=NULL;$mod->decorate($tx,$el);$el->attrs['src']=Texy::prependRoot($image->URL,$this->root);if(!isset($el->attrs['alt'])){if($alt!==NULL)$el->attrs['alt']=$tx->typographyModule->postLine($alt);else$el->attrs['alt']=$this->defaultAlt;}if($hAlign){$var=$hAlign.'Class';if(!empty($this->$var)){$el->attrs['class'][]=$this->$var;}elseif(empty($tx->alignClasses[$hAlign])){$el->attrs['style']['float']=$hAlign;}else{$el->attrs['class'][]=$tx->alignClasses[$hAlign];}}if(!is_int($image->width)||!is_int($image->height)||$image->asMax){if($this->fileRoot===NULL&&isset($_SERVER['SCRIPT_FILENAME'])){$this->fileRoot=dirname($_SERVER['SCRIPT_FILENAME']).'/'.$this->root;}if(Texy::isRelative($image->URL)&&strpos($image->URL,'..')===FALSE){$file=rtrim($this->fileRoot,'/\\').'/'.$image->URL;if(is_file($file)){$size=getImageSize($file);if(is_array($size)){if($image->asMax){$ratio=1;if(is_int($image->width))$ratio=min($ratio,$image->width/$size[0]);if(is_int($image->height))$ratio=min($ratio,$image->height/$size[1]);$image->width=round($ratio*$size[0]);$image->height=round($ratio*$size[1]);}elseif(is_int($image->width)){$ratio=round($size[1]/$size[0]*$image->width);$image->height=round($size[1]/$size[0]*$image->width);}elseif(is_int($image->height)){$image->width=round($size[0]/$size[1]*$image->height);}else{$image->width=$size[0];$image->height=$size[1];}}}}}$el->attrs['width']=$image->width;$el->attrs['height']=$image->height;if($image->overURL!==NULL){$overSrc=Texy::prependRoot($image->overURL,$this->root);$el->attrs['onmouseover']='this.src=\''.addSlashes($overSrc).'\'';$el->attrs['onmouseout']='this.src=\''.addSlashes($el->attrs['src']).'\'';$el->attrs['onload']=str_replace('%i',addSlashes($overSrc),$this->onLoad);$tx->summary['preload'][]=$overSrc;}$tx->summary['images'][]=$el->attrs['src'];if($link)return$tx->linkModule->solve(NULL,$link,$el);return$el;}}class
TexyImage
extends
NObject4{var$URL;var$overURL;var$linkedURL;var$width;var$height;var$asMax;var$modifier;var$name;function
__construct(){$this->modifier=new
TexyModifier;}function
__clone(){if($this->modifier){$this->modifier=clone($this->modifier);}}}$GLOBALS['TexyLinkModule::$deadlock']=NULL;class
TexyLinkModule
extends
TexyModule{var$root='';var$imageOnClick='return !popupImage(this.href)';var$popupOnClick='return !popup(this.href)';var$forceNoFollow=FALSE;var$shorten=TRUE;var$references=array();function
__construct($texy){$this->texy=$texy;$texy->allowed['link/definition']=TRUE;$texy->addHandler('newReference',array($this,'solveNewReference'));$texy->addHandler('linkReference',array($this,'solve'));$texy->addHandler('linkEmail',array($this,'solveUrlEmail'));$texy->addHandler('linkURL',array($this,'solveUrlEmail'));$texy->addHandler('beforeParse',array($this,'beforeParse'));$texy->registerLinePattern(array($this,'patternReference'),'#(\[[^\[\]\*\n'.TEXY_MARK.']+\])#U','link/reference');$texy->registerLinePattern(array($this,'patternUrlEmail'),'#(?<=^|[\s([<:\x17])(?:https?://|www\.|ftp://)[a-z0-9.-][/a-z\d+\.~%&?@=_:;\#,-]+[/\w\d+~%?@=_\#]#iu','link/url');$texy->registerLinePattern(array($this,'patternUrlEmail'),'#(?<=^|[\s([<:\x17])'.TEXY_EMAIL.'#iu','link/email');}function
beforeParse($texy,&$text){$GLOBALS['TexyLinkModule::$deadlock']=array();if($texy->allowed['link/definition']){$text=preg_replace_callback('#^\[([^\[\]\#\?\*\n]+)\]: +(\S+)(\ .+)?'.TEXY_MODIFIER.'?\s*()$#mUu',array($this,'patternReferenceDef'),$text);}}function
patternReferenceDef($matches){list(,$mRef,$mLink,$mLabel,$mMod)=$matches;$link=new
TexyLink($mLink);$link->label=trim($mLabel);$link->modifier->setProperties($mMod);$this->checkLink($link);$this->addReference($mRef,$link);return'';}function
patternReference($parser,$matches){list(,$mRef)=$matches;$tx=$this->texy;$name=substr($mRef,1,-1);$link=$this->getReference($name);if(!$link){return$tx->invokeAroundHandlers('newReference',$parser,array($name));}$link->type=TEXY_LINK_BRACKET;if($link->label!=''){if(isset($GLOBALS['TexyLinkModule::$deadlock'][$link->name])){$content=$link->label;}else{$GLOBALS['TexyLinkModule::$deadlock'][$link->name]=TRUE;$el=TexyHtml::el();$lineParser=new
TexyLineParser($tx,$el);$lineParser->parse($link->label);$content=$el->toString($tx);unset($GLOBALS['TexyLinkModule::$deadlock'][$link->name]);}}else{$content=$this->textualUrl($link);$content=$this->texy->protect($content,TEXY_CONTENT_TEXTUAL);}return$tx->invokeAroundHandlers('linkReference',$parser,array($link,$content));}function
patternUrlEmail($parser,$matches,$name){list($mURL)=$matches;$link=new
TexyLink($mURL);$this->checkLink($link);return$this->texy->invokeAroundHandlers($name==='link/email'?'linkEmail':'linkURL',$parser,array($link));}function
addReference($name,$link){$link->name=TexyUtf::strtolower($name);$this->references[$link->name]=$link;}function
getReference($name){$name=TexyUtf::strtolower($name);if(isset($this->references[$name])){return
clone($this->references[$name]);}else{$pos=strpos($name,'?');if($pos===FALSE)$pos=strpos($name,'#');if($pos!==FALSE){$name2=substr($name,0,$pos);if(isset($this->references[$name2])){$link=clone($this->references[$name2]);$link->URL.=substr($name,$pos);return$link;}}}return
FALSE;}function
factoryLink($dest,$mMod,$label){$tx=$this->texy;$type=TEXY_LINK_COMMON;if(strlen($dest)>1&&$dest{0}==='['&&$dest{1}!=='*'){$type=TEXY_LINK_BRACKET;$dest=substr($dest,1,-1);$link=$this->getReference($dest);}elseif(strlen($dest)>1&&$dest{0}==='['&&$dest{1}==='*'){$type=TEXY_LINK_IMAGE;$dest=trim(substr($dest,2,-2));$image=$tx->imageModule->getReference($dest);if($image){$link=new
TexyLink($image->linkedURL===NULL?$image->URL:$image->linkedURL);$link->modifier=$image->modifier;}}if(empty($link)){$link=new
TexyLink(trim($dest));$this->checkLink($link);}if(strpos($link->URL,'%s')!==FALSE){$link->URL=str_replace('%s',urlencode($tx->stringToText($label)),$link->URL);}$link->modifier->setProperties($mMod);$link->type=$type;return$link;}function
solve($invocation,$link,$content=NULL){if($link->URL==NULL)return$content;$tx=$this->texy;$el=TexyHtml::el('a');if(empty($link->modifier)){$nofollow=$popup=FALSE;}else{$nofollow=isset($link->modifier->classes['nofollow']);$popup=isset($link->modifier->classes['popup']);unset($link->modifier->classes['nofollow'],$link->modifier->classes['popup']);$el->attrs['href']=NULL;$link->modifier->decorate($tx,$el);}if($link->type===TEXY_LINK_IMAGE){$el->attrs['href']=Texy::prependRoot($link->URL,$tx->imageModule->linkedRoot);$el->attrs['onclick']=$this->imageOnClick;}else{$el->attrs['href']=Texy::prependRoot($link->URL,$this->root);if($nofollow||($this->forceNoFollow&&strpos($el->attrs['href'],'//')!==FALSE))$el->attrs['rel']='nofollow';}if($popup)$el->attrs['onclick']=$this->popupOnClick;if($content!==NULL)$el->add($content);$tx->summary['links'][]=$el->attrs['href'];return$el;}function
solveUrlEmail($invocation,$link){$content=$this->textualUrl($link);$content=$this->texy->protect($content,TEXY_CONTENT_TEXTUAL);return$this->solve(NULL,$link,$content);}function
solveNewReference($invocation,$name){return
FALSE;}function
checkLink($link){if(strncasecmp($link->URL,'www.',4)===0){$link->URL='http://'.$link->URL;}elseif(preg_match('#'.TEXY_EMAIL.'$#iA',$link->URL)){$link->URL='mailto:'.$link->URL;}elseif(!$this->texy->checkURL($link->URL,'a')){$link->URL=NULL;}else{$link->URL=str_replace('&amp;','&',$link->URL);}}function
textualUrl($link){if($this->texy->obfuscateEmail&&preg_match('#^'.TEXY_EMAIL.'$#i',$link->raw)){return
str_replace('@',"&#64;<!---->",$link->raw);}if($this->shorten&&preg_match('#^(https?://|ftp://|www\.|/)#i',$link->raw)){if(strncasecmp($link->raw,'www.',4)===0)$parts=@parse_url('none://'.$link->raw);else$parts=@parse_url($link->raw);if($parts===FALSE)return$link->raw;$res='';if(isset($parts['scheme'])&&$parts['scheme']!=='none')$res.=$parts['scheme'].'://';if(isset($parts['host']))$res.=$parts['host'];if(isset($parts['path']))$res.=(strlen($parts['path'])>16?("/\xe2\x80\xa6".preg_replace('#^.*(.{0,12})$#U','$1',$parts['path'])):$parts['path']);if(isset($parts['query'])){$res.=strlen($parts['query'])>4?"?\xe2\x80\xa6":('?'.$parts['query']);}elseif(isset($parts['fragment'])){$res.=strlen($parts['fragment'])>4?"#\xe2\x80\xa6":('#'.$parts['fragment']);}return$res;}return$link->raw;}}define('TEXY_LINK_COMMON',1);define('TEXY_LINK_BRACKET',2);define('TEXY_LINK_IMAGE',3);class
TexyLink
extends
NObject4{var$URL;var$raw;var$modifier;var$type=TEXY_LINK_COMMON;var$label;var$name;function
__construct($URL){$this->URL=$URL;$this->raw=$URL;$this->modifier=new
TexyModifier;}function
__clone(){if($this->modifier){$this->modifier=clone($this->modifier);}}}class
TexyListModule
extends
TexyModule{var$bullets=array('*'=>array('\*\ ',0,''),'-'=>array('[\x{2013}-](?![>-])',0,''),'+'=>array('\+\ ',0,''),'1.'=>array('1\.\ ',1,'','\d{1,3}\.\ '),'1)'=>array('\d{1,3}\)\ ',1,''),'I.'=>array('I\.\ ',1,'upper-roman','[IVX]{1,4}\.\ '),'I)'=>array('[IVX]+\)\ ',1,'upper-roman'),'a)'=>array('[a-z]\)\ ',1,'lower-alpha'),'A)'=>array('[A-Z]\)\ ',1,'upper-alpha'),);function
__construct($texy){$this->texy=$texy;$texy->addHandler('beforeParse',array($this,'beforeParse'));$texy->allowed['list']=TRUE;$texy->allowed['list/definition']=TRUE;}function
beforeParse(){$RE=$REul=array();foreach($this->bullets
as$desc){$RE[]=$desc[0];if(!$desc[1])$REul[]=$desc[0];}$this->texy->registerBlockPattern(array($this,'patternList'),'#^(?:'.TEXY_MODIFIER_H.'\n)?'.'('.implode('|',$RE).')\ *\S.*$#mUu','list');$this->texy->registerBlockPattern(array($this,'patternDefList'),'#^(?:'.TEXY_MODIFIER_H.'\n)?'.'(\S.*)\:\ *'.TEXY_MODIFIER_H.'?\n'.'(\ ++)('.implode('|',$REul).')\ *\S.*$#mUu','list/definition');}function
patternList($parser,$matches){list(,$mMod,$mBullet)=$matches;$tx=$this->texy;$el=TexyHtml::el();$bullet=$min=NULL;foreach($this->bullets
as$type=>$desc)if(preg_match('#'.$desc[0].'#Au',$mBullet)){$bullet=isset($desc[3])?$desc[3]:$desc[0];$min=isset($desc[3])?2:1;$el->setName($desc[1]?'ol':'ul');$el->attrs['style']['list-style-type']=$desc[2];if($desc[1]){if($type[0]==='1'&&(int)$mBullet>1)$el->attrs['start']=(int)$mBullet;elseif($type[0]==='a'&&$mBullet[0]>'a')$el->attrs['start']=ord($mBullet[0])-96;elseif($type[0]==='A'&&$mBullet[0]>'A')$el->attrs['start']=ord($mBullet[0])-64;}break;}$mod=new
TexyModifier($mMod);$mod->decorate($tx,$el);$parser->moveBackward(1);while($elItem=$this->patternItem($parser,$bullet,FALSE,'li')){$el->add($elItem);}if($el->count()<$min)return
FALSE;$tx->invokeHandlers('afterList',array($parser,$el,$mod));return$el;}function
patternDefList($parser,$matches){list(,$mMod,,,,$mBullet)=$matches;$tx=$this->texy;$bullet=NULL;foreach($this->bullets
as$desc)if(preg_match('#'.$desc[0].'#Au',$mBullet)){$bullet=isset($desc[3])?$desc[3]:$desc[0];break;}$el=TexyHtml::el('dl');$mod=new
TexyModifier($mMod);$mod->decorate($tx,$el);$parser->moveBackward(2);$patternTerm='#^\n?(\S.*)\:\ *'.TEXY_MODIFIER_H.'?()$#mUA';while(TRUE){if($elItem=$this->patternItem($parser,$bullet,TRUE,'dd')){$el->add($elItem);continue;}if($parser->next($patternTerm,$matches)){list(,$mContent,$mMod)=$matches;$elItem=TexyHtml::el('dt');$mod=new
TexyModifier($mMod);$mod->decorate($tx,$elItem);$elItem->parseLine($tx,$mContent);$el->add($elItem);continue;}break;}$tx->invokeHandlers('afterDefinitionList',array($parser,$el,$mod));return$el;}function
patternItem($parser,$bullet,$indented,$tag){$tx=$this->texy;$spacesBase=$indented?('\ {1,}'):'';$patternItem="#^\n?($spacesBase)$bullet\\ *(\\S.*)?".TEXY_MODIFIER_H."?()$#mAUu";$matches=NULL;if(!$parser->next($patternItem,$matches))return
FALSE;list(,$mIndent,$mContent,$mMod)=$matches;$elItem=TexyHtml::el($tag);$mod=new
TexyModifier($mMod);$mod->decorate($tx,$elItem);$spaces='';$content=' '.$mContent;while($parser->next('#^(\n*)'.$mIndent.'(\ {1,'.$spaces.'})(.*)()$#Am',$matches)){list(,$mBlank,$mSpaces,$mContent)=$matches;if($spaces==='')$spaces=strlen($mSpaces);$content.="\n".$mBlank.$mContent;}$elItem->parseBlock($tx,$content,TRUE);if($elItem->offsetExists(0)&&is_a($elItem->offsetGet(0),'TexyHtml')){$tmp=$elItem->offsetGet(0);$tmp->setName(NULL);}return$elItem;}}class
TexyLongWordsModule
extends
TexyModule{var$wordLimit=20;var$consonants=array('b','c','d','f','g','h','j','k','l','m','n','p','q','r','s','t','v','w','x','z','B','C','D','F','G','H','J','K','L','M','N','P','Q','R','S','T','V','W','X','Z',"\xc4\x8d","\xc4\x8f","\xc5\x88","\xc5\x99","\xc5\xa1","\xc5\xa5","\xc5\xbe","\xc4\x8c","\xc4\x8e","\xc5\x87","\xc5\x98","\xc5\xa0","\xc5\xa4","\xc5\xbd");var$vowels=array('a','e','i','o','u','y','A','E','I','O','U','Y',"\xc3\xa1","\xc3\xa9","\xc4\x9b","\xc3\xad","\xc3\xb3","\xc3\xba","\xc5\xaf","\xc3\xbd","\xc3\x81","\xc3\x89","\xc4\x9a","\xc3\x8d","\xc3\x93","\xc3\x9a","\xc5\xae","\xc3\x9d");var$before_r=array('b','B','c','C','d','D','f','F','g','G','k','K','p','P','r','R','t','T','v','V',"\xc4\x8d","\xc4\x8c","\xc4\x8f","\xc4\x8e","\xc5\x99","\xc5\x98","\xc5\xa5","\xc5\xa4");var$before_l=array('b','B','c','C','d','D','f','F','g','G','k','K','l','L','p','P','t','T','v','V',"\xc4\x8d","\xc4\x8c","\xc4\x8f","\xc4\x8e","\xc5\xa5","\xc5\xa4");var$before_h=array('c','C','s','S');var$doubleVowels=array('a','A','o','O');function
__construct($texy){$this->texy=$texy;$this->consonants=array_flip($this->consonants);$this->vowels=array_flip($this->vowels);$this->before_r=array_flip($this->before_r);$this->before_l=array_flip($this->before_l);$this->before_h=array_flip($this->before_h);$this->doubleVowels=array_flip($this->doubleVowels);$texy->registerPostLine(array($this,'postLine'),'longwords');}function
postLine($text){return
preg_replace_callback('#[^\ \n\t\x14\x15\x16\x{2013}\x{2014}\x{ad}-]{'.$this->wordLimit.',}#u',array($this,'pattern'),$text);}function
pattern($matches){list($mWord)=$matches;$chars=array();preg_match_all('#['.TEXY_MARK.']+|.#u',$mWord,$chars);$chars=$chars[0];if(count($chars)<$this->wordLimit)return$mWord;$consonants=$this->consonants;$vowels=$this->vowels;$before_r=$this->before_r;$before_l=$this->before_l;$before_h=$this->before_h;$doubleVowels=$this->doubleVowels;$s=array();$trans=array();$s[]='';$trans[]=-1;foreach($chars
as$key=>$char){if(ord($char{0})<32)continue;$s[]=$char;$trans[]=$key;}$s[]='';$len=count($s)-2;$positions=array();$a=0;$last=1;while(++$a<$len){$hyphen=0;do{if($s[$a]==="\xC2\xA0"){$a++;continue
2;}if($s[$a]==='.'){$hyphen=1;break;}if(isset($consonants[$s[$a]])){if(isset($vowels[$s[$a+1]])){if(isset($vowels[$s[$a-1]]))$hyphen=1;break;}if(($s[$a]==='s')&&($s[$a-1]==='n')&&isset($consonants[$s[$a+1]])){$hyphen=2;break;}if(isset($consonants[$s[$a+1]])&&isset($vowels[$s[$a-1]])){if($s[$a+1]==='r'){$hyphen=isset($before_r[$s[$a]])?1:2;break;}if($s[$a+1]==='l'){$hyphen=isset($before_l[$s[$a]])?1:2;break;}if($s[$a+1]==='h'){$hyphen=isset($before_h[$s[$a]])?0:2;break;}$hyphen=2;break;}break;}if(($s[$a]==='u')&&isset($doubleVowels[$s[$a-1]])){$hyphen=2;break;}if(isset($vowels[$s[$a]])&&isset($vowels[$s[$a-1]])){$hyphen=1;break;}}while(0);if($hyphen===0&&($a-$last>$this->wordLimit*0.6))$positions[]=$last=$a-1;if($hyphen===1)$positions[]=$last=$a-1;if($hyphen===2){$positions[]=$last=$a;$a++;}}$a=end($positions);if(($a===$len-1)&&isset($consonants[$s[$len]]))array_pop($positions);$syllables=array();$last=0;foreach($positions
as$pos){if($pos-$last>$this->wordLimit*0.6){$syllables[]=implode('',array_splice($chars,0,$trans[$pos]-$trans[$last]));$last=$pos;}}$syllables[]=implode('',$chars);return
implode("\xC2\xAD",$syllables);;}}class
TexyPhraseModule
extends
TexyModule{var$tags=array('phrase/strong'=>'strong','phrase/em'=>'em','phrase/em-alt'=>'em','phrase/ins'=>'ins','phrase/del'=>'del','phrase/sup'=>'sup','phrase/sup-alt'=>'sup','phrase/sub'=>'sub','phrase/sub-alt'=>'sub','phrase/span'=>'a','phrase/span-alt'=>'a','phrase/cite'=>'cite','phrase/acronym'=>'acronym','phrase/acronym-alt'=>'acronym','phrase/code'=>'code','phrase/quote'=>'q','phrase/quicklink'=>'a',);var$linksAllowed=TRUE;function
__construct($texy){$this->texy=$texy;$texy->addHandler('phrase',array($this,'solve'));$texy->registerLinePattern(array($this,'patternPhrase'),'#(?<!\*)\*\*\*(?![\s*])(.+)'.TEXY_MODIFIER.'?(?<![\s*])\*\*\*(?!\*)'.TEXY_LINK.'??()#Uus','phrase/strong+em');$texy->registerLinePattern(array($this,'patternPhrase'),'#(?<!\*)\*\*(?![\s*])(.+)'.TEXY_MODIFIER.'?(?<![\s*])\*\*(?!\*)'.TEXY_LINK.'??()#Uus','phrase/strong');$texy->registerLinePattern(array($this,'patternPhrase'),'#(?<![/:])\/\/(?![\s/])(.+)'.TEXY_MODIFIER.'?(?<![\s/])\/\/(?!\/)'.TEXY_LINK.'??()#Uus','phrase/em');$texy->registerLinePattern(array($this,'patternPhrase'),'#(?<!\*)\*(?![\s*])(.+)'.TEXY_MODIFIER.'?(?<![\s*])\*(?!\*)'.TEXY_LINK.'??()#Uus','phrase/em-alt');$texy->registerLinePattern(array($this,'patternPhrase'),'#(?<!\+)\+\+(?![\s+])([^\r\n]+)'.TEXY_MODIFIER.'?(?<![\s+])\+\+(?!\+)()#Uu','phrase/ins');$texy->registerLinePattern(array($this,'patternPhrase'),'#(?<![<-])\-\-(?![\s>-])([^\r\n]+)'.TEXY_MODIFIER.'?(?<![\s<-])\-\-(?![>-])()#Uu','phrase/del');$texy->registerLinePattern(array($this,'patternPhrase'),'#(?<!\^)\^\^(?![\s^])([^\r\n]+)'.TEXY_MODIFIER.'?(?<![\s^])\^\^(?!\^)()#Uu','phrase/sup');$texy->registerLinePattern(array($this,'patternSupSub'),'#(?<=[a-z0-9])\^(-?[0-9]{1,4})(?![a-z0-9])#Uui','phrase/sup-alt');$texy->registerLinePattern(array($this,'patternPhrase'),'#(?<!\_)\_\_(?![\s_])([^\r\n]+)'.TEXY_MODIFIER.'?(?<![\s_])\_\_(?!\_)()#Uu','phrase/sub');$texy->registerLinePattern(array($this,'patternSupSub'),'#(?<=[a-z])\_([0-9]{1,3})(?![a-z0-9])#Uui','phrase/sub-alt');$texy->registerLinePattern(array($this,'patternPhrase'),'#(?<!\")\"(?!\s)([^\"\r]+)'.TEXY_MODIFIER.'?(?<!\s)\"(?!\")'.TEXY_LINK.'??()#Uu','phrase/span');$texy->registerLinePattern(array($this,'patternPhrase'),'#(?<!\~)\~(?!\s)([^\~\r]+)'.TEXY_MODIFIER.'?(?<!\s)\~(?!\~)'.TEXY_LINK.'??()#Uu','phrase/span-alt');$texy->registerLinePattern(array($this,'patternPhrase'),'#(?<!\~)\~\~(?![\s~])([^\r\n]+)'.TEXY_MODIFIER.'?(?<![\s~])\~\~(?!\~)'.TEXY_LINK.'??()#Uu','phrase/cite');$texy->registerLinePattern(array($this,'patternPhrase'),'#(?<!\>)\>\>(?![\s>])([^\r\n]+)'.TEXY_MODIFIER.'?(?<![\s<])\<\<(?!\<)'.TEXY_LINK.'??()#Uu','phrase/quote');$texy->registerLinePattern(array($this,'patternPhrase'),'#(?<!\")\"(?!\s)([^\"\r\n]+)'.TEXY_MODIFIER.'?(?<!\s)\"(?!\")\(\((.+)\)\)()#Uu','phrase/acronym');$texy->registerLinePattern(array($this,'patternPhrase'),'#(?<!['.TEXY_CHAR.'])(['.TEXY_CHAR.']{2,})()\(\((.+)\)\)#Uu','phrase/acronym-alt');$texy->registerLinePattern(array($this,'patternNoTexy'),'#(?<!\')\'\'(?![\s\'])([^'.TEXY_MARK.'\r\n]*)(?<![\s\'])\'\'(?!\')()#Uu','phrase/notexy');$texy->registerLinePattern(array($this,'patternPhrase'),'#\`(\S[^'.TEXY_MARK.'\r\n]*)'.TEXY_MODIFIER.'?(?<!\s)\`'.TEXY_LINK.'??()#Uu','phrase/code');$texy->registerLinePattern(array($this,'patternPhrase'),'#(['.TEXY_CHAR.'0-9@\#$%&.,_-]+)()(?=:\[)'.TEXY_LINK.'()#Uu','phrase/quicklink');$texy->allowed['phrase/ins']=FALSE;$texy->allowed['phrase/del']=FALSE;$texy->allowed['phrase/sup']=FALSE;$texy->allowed['phrase/sub']=FALSE;$texy->allowed['phrase/cite']=FALSE;}function
patternPhrase($parser,$matches,$phrase){list(,$mContent,$mMod,$mLink)=$matches;$tx=$this->texy;$mod=new
TexyModifier($mMod);$link=NULL;$parser->again=$phrase!=='phrase/code'&&$phrase!=='phrase/quicklink';if($phrase==='phrase/span'||$phrase==='phrase/span-alt'){if($mLink==NULL){if(!$mMod)return
FALSE;}else{$link=$tx->linkModule->factoryLink($mLink,$mMod,$mContent);}}elseif($phrase==='phrase/acronym'||$phrase==='phrase/acronym-alt'){$mod->title=trim(Texy::unescapeHtml($mLink));}elseif($phrase==='phrase/quote'){$mod->cite=$tx->blockQuoteModule->citeLink($mLink);}elseif($mLink!=NULL){$link=$tx->linkModule->factoryLink($mLink,NULL,$mContent);}return$tx->invokeAroundHandlers('phrase',$parser,array($phrase,$mContent,$mod,$link));}function
patternSupSub($parser,$matches,$phrase){list(,$mContent)=$matches;$mod=new
TexyModifier();$link=NULL;$mContent=str_replace('-',"\xE2\x88\x92",$mContent);return$this->texy->invokeAroundHandlers('phrase',$parser,array($phrase,$mContent,$mod,$link));}function
patternNoTexy($parser,$matches){list(,$mContent)=$matches;return$this->texy->protect(Texy::escapeHtml($mContent),TEXY_CONTENT_TEXTUAL);}function
solve($invocation,$phrase,$content,$mod,$link){$tx=$this->texy;$tag=isset($this->tags[$phrase])?$this->tags[$phrase]:NULL;if($tag==='a')$tag=$link&&$this->linksAllowed?NULL:'span';if($phrase==='phrase/code')$content=$tx->protect(Texy::escapeHtml($content),TEXY_CONTENT_TEXTUAL);if($phrase==='phrase/strong+em'){$el=TexyHtml::el($this->tags['phrase/strong']);$el->create($this->tags['phrase/em'],$content);$mod->decorate($tx,$el);}elseif($tag){$el=TexyHtml::el($tag);$el->setText($content);$mod->decorate($tx,$el);if($tag==='q')$el->attrs['cite']=$mod->cite;}else{$el=$content;}if($link&&$this->linksAllowed)return$tx->linkModule->solve(NULL,$link,$el);return$el;}}class
TexyBlockQuoteModule
extends
TexyModule{function
__construct($texy){$this->texy=$texy;$texy->registerBlockPattern(array($this,'pattern'),'#^(?:'.TEXY_MODIFIER_H.'\n)?\>(\ +|:)(\S.*)$#mU','blockquote');}function
pattern($parser,$matches){list(,$mMod,$mPrefix,$mContent)=$matches;$tx=$this->texy;$el=TexyHtml::el('blockquote');$mod=new
TexyModifier($mMod);$mod->decorate($tx,$el);$content='';$spaces='';do{if($mPrefix===':'){$mod->cite=$tx->blockQuoteModule->citeLink($mContent);$content.="\n";}else{if($spaces==='')$spaces=max(1,strlen($mPrefix));$content.=$mContent."\n";}if(!$parser->next("#^>(?:|(\\ {1,$spaces}|:)(.*))()$#mA",$matches))break;list(,$mPrefix,$mContent)=$matches;}while(TRUE);$el->attrs['cite']=$mod->cite;$el->parseBlock($tx,$content,$parser->isIndented());if(!$el->count())return
FALSE;$tx->invokeHandlers('afterBlockquote',array($parser,$el,$mod));return$el;}function
citeLink($link){$tx=$this->texy;if($link==NULL)return
NULL;if($link{0}==='['){$link=substr($link,1,-1);$ref=$tx->linkModule->getReference($link);if($ref)return
Texy::prependRoot($ref->URL,$tx->linkModule->root);}if(!$tx->checkURL($link,'c'))return
NULL;if(strncasecmp($link,'www.',4)===0)return'http://'.$link;return
Texy::prependRoot($link,$tx->linkModule->root);}}class
TexyScriptModule
extends
TexyModule{var$handler;var$separator=',';function
__construct($texy){$this->texy=$texy;$texy->addHandler('script',array($this,'solve'));$texy->registerLinePattern(array($this,'pattern'),'#\{\{([^'.TEXY_MARK.']+)\}\}()#U','script');}function
pattern($parser,$matches){list(,$mContent)=$matches;$cmd=trim($mContent);if($cmd==='')return
FALSE;$args=$raw=NULL;if(preg_match('#^([a-z_][a-z0-9_-]*)\s*(?:\(([^()]*)\)|:(.*))$#iu',$cmd,$matches)){$cmd=$matches[1];$raw=isset($matches[3])?trim($matches[3]):trim($matches[2]);if($raw==='')$args=array();else$args=preg_split('#\s*'.preg_quote($this->separator,'#').'\s*#u',$raw);}if($this->handler){if(is_callable(array($this->handler,$cmd))){array_unshift($args,$parser);return
call_user_func_array(array($this->handler,$cmd),$args);}if(is_callable($this->handler))return
call_user_func_array($this->handler,array($parser,$cmd,$args,$raw));}return$this->texy->invokeAroundHandlers('script',$parser,array($cmd,$args,$raw));}function
solve($invocation,$cmd,$args,$raw){if($cmd==='texy'){if(!$args)return
FALSE;switch($args[0]){case'nofollow':$this->texy->linkModule->forceNoFollow=TRUE;break;}return'';}else{return
FALSE;}}}class
TexyEmoticonModule
extends
TexyModule{var$icons=array(':-)'=>'smile.gif',':-('=>'sad.gif',';-)'=>'wink.gif',':-D'=>'biggrin.gif','8-O'=>'eek.gif','8-)'=>'cool.gif',':-?'=>'confused.gif',':-x'=>'mad.gif',':-P'=>'razz.gif',':-|'=>'neutral.gif',);var$class;var$root;var$fileRoot;function
__construct($texy){$this->texy=$texy;$texy->allowed['emoticon']=FALSE;$texy->addHandler('emoticon',array($this,'solve'));$texy->addHandler('beforeParse',array($this,'beforeParse'));}function
beforeParse(){if(empty($this->texy->allowed['emoticon']))return;krsort($this->icons);$pattern=array();foreach($this->icons
as$key=>$foo)$pattern[]=preg_quote($key,'#').'+';$this->texy->registerLinePattern(array($this,'pattern'),'#(?<=^|[\\x00-\\x20])('.implode('|',$pattern).')#','emoticon');}function
pattern($parser,$matches){$match=$matches[0];$tx=$this->texy;foreach($this->icons
as$emoticon=>$foo){if(strncmp($match,$emoticon,strlen($emoticon))===0){return$tx->invokeAroundHandlers('emoticon',$parser,array($emoticon,$match));}}return
FALSE;}function
solve($invocation,$emoticon,$raw){$tx=$this->texy;$file=$this->icons[$emoticon];$el=TexyHtml::el('img');$el->attrs['src']=Texy::prependRoot($file,$this->root===NULL?$tx->imageModule->root:$this->root);$el->attrs['alt']=$raw;$el->attrs['class'][]=$this->class;$file=rtrim($this->fileRoot===NULL?$tx->imageModule->fileRoot:$this->fileRoot,'/\\').'/'.$file;if(is_file($file)){$size=getImageSize($file);if(is_array($size)){$el->attrs['width']=$size[0];$el->attrs['height']=$size[1];}}$tx->summary['images'][]=$el->attrs['src'];return$el;}}class
TexyTableModule
extends
TexyModule{var$oddClass;var$evenClass;function
__construct($texy){$this->texy=$texy;$texy->registerBlockPattern(array($this,'patternTable'),'#^(?:'.TEXY_MODIFIER_HV.'\n)?'.'\|.*()$#mU','table');}function
patternTable($parser,$matches){list(,$mMod)=$matches;$tx=$this->texy;$el=TexyHtml::el('table');$mod=new
TexyModifier($mMod);$mod->decorate($tx,$el);$parser->moveBackward();if($parser->next('#^\|(\#|\=){2,}(?!\\1)(.*)\\1*\|? *'.TEXY_MODIFIER_H.'?()$#Um',$matches)){list(,,$mContent,$mMod)=$matches;$caption=$el->create('caption');$mod=new
TexyModifier($mMod);$mod->decorate($tx,$caption);$caption->parseLine($tx,$mContent);}$isHead=FALSE;$colModifier=array();$prevRow=array();$rowCounter=0;$colCounter=0;$elPart=NULL;while(TRUE){if($parser->next('#^\|[+-]{3,}$#Um',$matches)){$isHead=!$isHead;$prevRow=array();continue;}if($parser->next('#^\|(.*)(?:|\|\ *'.TEXY_MODIFIER_HV.'?)()$#U',$matches)){if($rowCounter===0&&!$isHead&&$parser->next('#^\|[+-]{3,}$#Um',$foo)){$isHead=TRUE;$parser->moveBackward();}if($elPart===NULL){$elPart=$el->create($isHead?'thead':'tbody');}elseif(!$isHead&&$elPart->getName()==='thead'){$this->finishPart($elPart);$elPart=$el->create('tbody');}list(,$mContent,$mMod)=$matches;$elRow=TexyHtml::el('tr');$mod=new
TexyModifier($mMod);$mod->decorate($tx,$elRow);$rowClass=$rowCounter
%
2===0?$this->oddClass:$this->evenClass;if($rowClass&&!isset($mod->classes[$this->oddClass])&&!isset($mod->classes[$this->evenClass])){$elRow->attrs['class'][]=$rowClass;}$col=0;$elCell=NULL;$mContent=str_replace('\\|','&#x7C;',$mContent);foreach(explode('|',$mContent)as$cell){if($cell===''&&$elCell){$elCell->colSpan++;unset($prevRow[$col]);$col++;continue;}if(isset($prevRow[$col])&&preg_match('#\^\ *$|\*??(.*)\ +\^$#AU',$cell,$matches)){$prevRow[$col]->rowSpan++;$matches[]='';$prevRow[$col]->text.="\n".$matches[1];$col+=$prevRow[$col]->colSpan;$elCell=NULL;continue;}if(!preg_match('#(\*??)\ *'.TEXY_MODIFIER_HV.'??(.*)'.TEXY_MODIFIER_HV.'?\ *()$#AU',$cell,$matches))continue;list(,$mHead,$mModCol,$mContent,$mMod)=$matches;if($mModCol){$colModifier[$col]=new
TexyModifier($mModCol);}if(isset($colModifier[$col]))$mod=clone($colModifier[$col]);else$mod=new
TexyModifier;$mod->setProperties($mMod);$elCell=new
TexyTableCellElement;$elCell->setName($isHead||($mHead==='*')?'th':'td');$mod->decorate($tx,$elCell);$elCell->text=$mContent;$elRow->add($elCell);$prevRow[$col]=$elCell;$col++;}while($col<$colCounter){$elCell=new
TexyTableCellElement;$elCell->setName($isHead?'th':'td');if(isset($colModifier[$col])){$colModifier[$col]->decorate($tx,$elCell);}$elRow->add($elCell);$prevRow[$col]=$elCell;$col++;}$colCounter=$col;if($elRow->count()){$elPart->add($elRow);$rowCounter++;}else{foreach($prevRow
as$elCell)$elCell->rowSpan--;}continue;}break;}if($elPart===NULL){return
FALSE;}if($elPart->getName()==='thead'){$elPart->setName('tbody');}$this->finishPart($elPart);$tx->invokeHandlers('afterTable',array($parser,$el,$mod));return$el;}function
finishPart($elPart){$tx=$this->texy;foreach($elPart->getChildren()as$elRow){foreach($elRow->getChildren()as$elCell){if($elCell->colSpan>1)$elCell->attrs['colspan']=$elCell->colSpan;if($elCell->rowSpan>1){$elCell->attrs['rowspan']=$elCell->rowSpan;$text=Texy::outdent($elCell->text);if(strpos($text,"\n")!==FALSE){$elCell->parseBlock($tx,$text);continue;}}$elCell->parseLine($tx,trim($elCell->text));if($elCell->getText()==='')$elCell->setText("\xC2\xA0");}}}}class
TexyTableCellElement
extends
TexyHtml{var$colSpan=1;var$rowSpan=1;var$text;}$GLOBALS['TexyTypographyModule::$locales']=array('cs'=>array('singleQuotes'=>array("\xe2\x80\x9a","\xe2\x80\x98"),'doubleQuotes'=>array("\xe2\x80\x9e","\xe2\x80\x9c"),),'en'=>array('singleQuotes'=>array("\xe2\x80\x98","\xe2\x80\x99"),'doubleQuotes'=>array("\xe2\x80\x9c","\xe2\x80\x9d"),),'fr'=>array('singleQuotes'=>array("\xe2\x80\xb9","\xe2\x80\xba"),'doubleQuotes'=>array("\xc2\xab","\xc2\xbb"),),'de'=>array('singleQuotes'=>array("\xe2\x80\x9a","\xe2\x80\x98"),'doubleQuotes'=>array("\xe2\x80\x9e","\xe2\x80\x9c"),),'pl'=>array('singleQuotes'=>array("\xe2\x80\x9a","\xe2\x80\x99"),'doubleQuotes'=>array("\xe2\x80\x9e","\xe2\x80\x9d"),),);class
TexyTypographyModule
extends
TexyModule{var$locale='cs';var$pattern,$replace;function
__construct($texy){$this->texy=$texy;$texy->registerPostLine(array($this,'postLine'),'typography');$texy->addHandler('beforeParse',array($this,'beforeParse'));}function
beforeParse($texy,&$text){if(isset($GLOBALS['TexyTypographyModule::$locales'][$this->locale]))$locale=$GLOBALS['TexyTypographyModule::$locales'][$this->locale];else$locale=$GLOBALS['TexyTypographyModule::$locales']['en'];$pairs=array('#(?<![.\x{2026}])\.{3,4}(?![.\x{2026}])#mu'=>"\xe2\x80\xa6",'#(?<=[\d ]|^)-(?=[\d ]|$)#'=>"\xe2\x80\x93",'#(?<=[^!*+,/:;<=>@\\\\_|-])--(?=[^!*+,/:;<=>@\\\\_|-])#'=>"\xe2\x80\x93",'#,-#'=>",\xe2\x80\x93",'#(?<!\d)(\d{1,2}\.) (\d{1,2}\.) (\d\d)#'=>"\$1\xc2\xa0\$2\xc2\xa0\$3",'#(?<!\d)(\d{1,2}\.) (\d{1,2}\.)#'=>"\$1\xc2\xa0\$2",'# --- #'=>"\xc2\xa0\xe2\x80\x94 ",'# ([\x{2013}\x{2014}])#u'=>"\xc2\xa0\$1",'# <-{1,2}> #'=>" \xe2\x86\x94 ",'#-{1,}> #'=>" \xe2\x86\x92 ",'# <-{1,}#'=>" \xe2\x86\x90 ",'#={1,}> #'=>" \xe2\x87\x92 ",'#(\d+)( ?)x\\2(?=\d)#'=>"\$1\xc3\x97",'#(?<=\d)x(?= |,|.|$)#m'=>"\xc3\x97",'#(\S ?)\(TM\)#i'=>"\$1\xe2\x84\xa2",'#(\S ?)\(R\)#i'=>"\$1\xc2\xae",'#\(C\)( ?\S)#i'=>"\xc2\xa9\$1",'#\(EUR\)#'=>"\xe2\x82\xac",'#(\d{1,3}) (?=\d{3})#'=>"\$1\xc2\xa0",'#(?<=[^\s\x17])\s+([\x17-\x1F]+)(?=\s)#u'=>"\$1",'#(?<=\s)([\x17-\x1F]+)\s+#u'=>"\$1",'#(?<=.{50})\s+(?=[\x17-\x1F]*\S{1,6}[\x17-\x1F]*$)#us'=>"\xc2\xa0",'#(?<=^| |\.|,|-|\+|\x16|\()([\x17-\x1F]*\d+\.?[\x17-\x1F]*)\s+(?=[\x17-\x1F]*[%'.TEXY_CHAR.'\x{b0}-\x{be}\x{2020}-\x{214f}])#mu'=>"\$1\xc2\xa0",'#(?<=^|[^0-9'.TEXY_CHAR.'])([\x17-\x1F]*[ksvzouiKSVZOUIA][\x17-\x1F]*)\s+(?=[\x17-\x1F]*[0-9'.TEXY_CHAR.'])#mus'=>"\$1\xc2\xa0",'#(?<!"|\w)"(?!\ |")(.+)(?<!\ |")"(?!")()#U'=>$locale['doubleQuotes'][0].'$1'.$locale['doubleQuotes'][1],'#(?<!\'|\w)\'(?!\ |\')(.+)(?<!\ |\')\'(?!\')()#Uu'=>$locale['singleQuotes'][0].'$1'.$locale['singleQuotes'][1],);$this->pattern=array_keys($pairs);$this->replace=array_values($pairs);}function
postLine($text){return
preg_replace($this->pattern,$this->replace,$text);}}class
TexyHtmlOutputModule
extends
TexyModule{var$xhtml=TRUE;var$indent=TRUE;var$baseIndent=0;var$lineWrap=80;var$removeOptional=TRUE;var$space;var$tagUsed;var$tagStack;var$baseDTD;function
__construct($texy){$this->texy=$texy;$texy->addHandler('postProcess',array($this,'postProcess'));}function
postProcess($texy,&$s){$this->space=$this->baseIndent;$this->tagStack=array();$this->tagUsed=array();$dtd=$GLOBALS['TexyHtml::$dtd'];$this->baseDTD=$dtd['div'][1]+$dtd['html'][1]+$dtd['body'][1]+array('html'=>1);$s=preg_replace_callback('#(.*)<(?:(!--.*--)|(/?)([a-z][a-z0-9._:-]*)(|[ \n].*)\s*(/?))>()#Uis',array($this,'cb'),$s.'</end/>');foreach($this->tagStack
as$item)$s.=$item['close'];$s=preg_replace("#[\t ]+(\n|\r|$)#",'$1',$s);$s=str_replace("\r\r","\n",$s);$s=strtr($s,"\r","\n");$s=preg_replace("#\\x07 *#",'',$s);$s=preg_replace("#\\t? *\\x08#",'',$s);if($this->lineWrap>0)$s=preg_replace_callback('#^(\t*)(.*)$#m',array($this,'wrap'),$s);if(!$this->xhtml&&$this->removeOptional)$s=preg_replace('#\\s*</(colgroup|dd|dt|li|option|p|td|tfoot|th|thead|tr)>#u','',$s);}function
cb($matches){list(,$mText,$mComment,$mEnd,$mTag,$mAttr,$mEmpty)=$matches;$s='';if($mText!==''){$item=reset($this->tagStack);if($item&&!isset($item['dtdContent']['%DATA'])){}elseif(!empty($this->tagUsed['pre'])||!empty($this->tagUsed['textarea'])||!empty($this->tagUsed['script']))$s=Texy::freezeSpaces($mText);else$s=preg_replace('#[ \n]+#',' ',$mText);}if($mComment)return$s.'<'.Texy::freezeSpaces($mComment).'>';$mEmpty=$mEmpty||isset($GLOBALS['TexyHtml::$emptyElements'][$mTag]);if($mEmpty&&$mEnd)return$s;if($mEnd){if(empty($this->tagUsed[$mTag]))return$s;$tmp=array();$back=TRUE;foreach($this->tagStack
as$i=>$item){$tag=$item['tag'];$s.=$item['close'];$this->space-=$item['indent'];$this->tagUsed[$tag]--;$back=$back&&isset($GLOBALS['TexyHtml::$inlineElements'][$tag]);unset($this->tagStack[$i]);if($tag===$mTag)break;array_unshift($tmp,$item);}if(!$back||!$tmp)return$s;$item=reset($this->tagStack);if($item)$dtdContent=$item['dtdContent'];else$dtdContent=$this->baseDTD;if(!isset($dtdContent[$tmp[0]['tag']]))return$s;foreach($tmp
as$item){$s.=$item['open'];$this->space+=$item['indent'];$this->tagUsed[$item['tag']]++;array_unshift($this->tagStack,$item);}}else{$dtdContent=$this->baseDTD;$dtd=$GLOBALS['TexyHtml::$dtd'];if(!isset($dtd[$mTag])){$allowed=TRUE;$item=reset($this->tagStack);if($item)$dtdContent=$item['dtdContent'];}else{foreach($this->tagStack
as$i=>$item){$dtdContent=$item['dtdContent'];if(isset($dtdContent[$mTag]))break;$tag=$item['tag'];if($item['close']&&(!isset($GLOBALS['TexyHtml::$optionalEnds'][$tag])&&!isset($GLOBALS['TexyHtml::$inlineElements'][$tag])))break;$s.=$item['close'];$this->space-=$item['indent'];$this->tagUsed[$tag]--;unset($this->tagStack[$i]);$dtdContent=$this->baseDTD;}$allowed=isset($dtdContent[$mTag]);if($allowed&&isset($GLOBALS['TexyHtml::$prohibits'][$mTag])){foreach($GLOBALS['TexyHtml::$prohibits'][$mTag]as$pTag)if(!empty($this->tagUsed[$pTag])){$allowed=FALSE;break;}}}if($mEmpty){if(!$allowed)return$s;if($this->xhtml)$mAttr.=" /";if($this->indent&&$mTag==='br')return
rtrim($s).'<'.$mTag.$mAttr.">\n".str_repeat("\t",max(0,$this->space-1))."\x07";if($this->indent&&!isset($GLOBALS['TexyHtml::$inlineElements'][$mTag])){$space="\r".str_repeat("\t",$this->space);return$s.$space.'<'.$mTag.$mAttr.'>'.$space;}return$s.'<'.$mTag.$mAttr.'>';}$open=NULL;$close=NULL;$indent=0;if($allowed){$open='<'.$mTag.$mAttr.'>';if(!empty($dtd[$mTag][1]))$dtdContent=$dtd[$mTag][1];if($this->indent&&!isset($GLOBALS['TexyHtml::$inlineElements'][$mTag])){$close="\x08".'</'.$mTag.'>'."\n".str_repeat("\t",$this->space);$s.="\n".str_repeat("\t",$this->space++).$open."\x07";$indent=1;}else{$close='</'.$mTag.'>';$s.=$open;}}$item=array('tag'=>$mTag,'open'=>$open,'close'=>$close,'dtdContent'=>$dtdContent,'indent'=>$indent,);array_unshift($this->tagStack,$item);$tmp=&$this->tagUsed[$mTag];$tmp++;}return$s;}function
wrap($m){list(,$space,$s)=$m;return$space.wordwrap($s,$this->lineWrap,"\n".$space);}}if(!class_exists('LogicException')){class
LogicException
extends
Exception{}}if(!class_exists('InvalidArgumentException')){class
InvalidArgumentException
extends
LogicException{}}if(!class_exists('RuntimeException')){class
RuntimeException
extends
Exception{}}if(!class_exists('UnexpectedValueException')){class
UnexpectedValueException
extends
RuntimeException{}}if(!class_exists('NotSupportedException')){class
NotSupportedException
extends
LogicException{}}if(!class_exists('InvalidStateException')){class
InvalidStateException
extends
RuntimeException{}}if(version_compare(PHP_VERSION,'4.3.3','<')){die('Texy needs PHP 4.3.3 or newer.');}if(function_exists('mb_get_info')){if(mb_get_info('func_overload')&2&&substr(mb_get_info('internal_encoding'),0,1)==='U'){mb_internal_encoding('pass');trigger_error("Texy: mb_internal_encoding changed to 'pass'",E_USER_WARNING);}}