<?php
// $Id: texy_syntaxhighlighting.admin.inc,v 1.1.2.1 2008/01/06 12:08:39 havran Exp $

/**
 * @file
 * Admin page callbacks for the Texy Syntaxhighlighting.
 */

/**
 * Form builder; Configure Syntaxhighlighting settings for Texy!.
 *
 * @ingroup forms
 * @see system_settings_form().
 */
function texy_form_syntaxhighlighting_settings() {
  $module_path = drupal_get_path('module', 'texy_syntaxhighlighting');

  $form['highlighterset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Syntax highlighting with FSHL'),
    '#collapsible' => TRUE,
    '#weight' => -1,
  );
  $form['highlighterset']['texy_syntaxhighlighting_use'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use source code syntax highlighting'),
    '#description' => t('Check this option if you would like to highlight entered source code.'),
    '#default_value' => variable_get('texy_syntaxhighlighting_use', FALSE),
  );
  /*
  $form['highlighterset']['texy_syntaxhighlighting_name'] = array(
    '#type' => 'textfield',
    '#size' => '20',
    '#title' => t('Syntax highlighter\'s name'),
    '#description' => t('Name of a tool which you wish to use for syntax highlighting. For example, if you plan to use <a href="http://hvge.sk/scripts/fshl/">FSHL</a> you should enter <code>fshl</code> and install the tool into the <code>!module_path/lib/fshl</code> directory, together with a corresponding <code>!module_path/lib/fshl.php</code> file which would be a PHP implementation of the highlighter.', array('!module_path' => $module_path)),
    '#default_value' => variable_get('texy_syntaxhighlighting_name', 'fshl'),
  );
  $form['highlighterset']['texy_syntaxhighlighting_css_path'] = array(
    '#type' => 'textfield',
    '#title' => t("Syntax highlighter's stylesheet"),
    '#description' => t('Path to a stylesheet wich will be used together with the syntax highlighter. For example, if you plan to use <a href="http://hvge.sk/scripts/fshl/">FSHL</a> you should enter <code>fshl/styles/COHEN_style.css</code>.'),
    '#default_value' => variable_get('texy_syntaxhighlighting_css_path', 'fshl/styles/COHEN_style.css'),
  );
  // */

  return system_settings_form($form);
}

function _sh_help() {
  $str = "<h3>Examples for enabling syntaxhighlighting.</h3>
<p>In Texy!, HTML tag and Square brackets syntax you can use <em>PHP, HTML, CSS, JS (or JAVASCRIPT), CPP, JAVA, SQL</em> and <em>PY (or PYTHON)</em> (lowercase), for language which you want highlight.</p>

<h3>You can use:</h3>

<h4>Texy! syntax:</h4>
<code><pre>
/--code php
print $foo;
\--
</pre></code>

<h4>HTML tag syntax:</h4>
<code><pre>
&lt;php&gt;
print $foo;
&lt;/php&gt;
</pre></code>

<h4>Square brackets syntax:</h4>
<code><pre>
[php]
print $foo;
[/php]
</pre></code>

<h4>PHP syntax (for PHP only):</h4>
<code><pre>
&lt;?php
print $foo;
?&gt;
</pre></code>

<h4>Script syntax (for JavaScript only):</h4>
<code><pre>
&lt;script&gt;
alert(foo);
&lt;/script&gt;
</pre></code>

<p>or you ca use:</p>

<code><pre>
&lt;script type=\"text/javascript\"&gt;
alert(foo);
&lt;/script&gt;
</pre></code>";

  return $str;
}
