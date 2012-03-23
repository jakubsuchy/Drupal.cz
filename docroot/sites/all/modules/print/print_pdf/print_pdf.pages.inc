<?php
// $Id: print_pdf.pages.inc,v 1.3.2.29 2009/03/12 20:22:54 jcnventura Exp $

/**
 * @file
 * Generates the PDF versions of the pages
 *
 * This file is included by the print_pdf module and includes the
 * functions that interface with the PDF generation packages.
 */

require_once(drupal_get_path('module', 'print') .'/print.pages.inc');

/**
 * Generate a PDF version of the printer-friendly page
 *
 * @see print_controller()
 * @see _print_get_template()
 * @see _print_pdf_dompdf()
 * @see _print_pdf_tcpdf()
 */
function print_pdf_controller() {
  global $base_url, $language;

  // Disable caching for generated PDFs, as Drupal doesn't ouput the proper headers from the cache
  $GLOBALS['conf']['cache'] = FALSE;

  $args = func_get_args();
  // Remove the printpdf/ prefix
  $path = implode('/', $args);
  $cid = isset($_GET['comment']) ? (int)$_GET['comment'] : NULL;

  $print_pdf_pdf_tool = variable_get('print_pdf_pdf_tool', PRINT_PDF_PDF_TOOL_DEFAULT);

  $print = print_controller($path, $cid, PRINT_PDF_FORMAT);
  if ($print === FALSE) {
    return;
  }

  // Img elements must be set to absolute
  $pattern = '!<(img\s[^>]*?)>!is';
  $print['content'] = preg_replace_callback($pattern, '_print_rewrite_urls', $print['content']);
  $print['logo'] = preg_replace_callback($pattern, '_print_rewrite_urls', $print['logo']);
  $print['footer_message'] = preg_replace_callback($pattern, '_print_rewrite_urls', $print['footer_message']);
  // And converted from private to public paths
  $file_downloads = variable_get('file_downloads', FILE_DOWNLOADS_PUBLIC);
  if ($file_downloads == FILE_DOWNLOADS_PRIVATE) {
    switch (variable_get('language_negotiation', LANGUAGE_NEGOTIATION_NONE)) {
      case LANGUAGE_NEGOTIATION_PATH_DEFAULT:
      case LANGUAGE_NEGOTIATION_PATH:
        $lang = $language->language;
        break;
      default:
        $lang = '';
        break;
    }
    $pattern = "!(<img\s[^>]*?src\s*?=\s*?['\"]?)${base_url}/(?:(?:index.php)?\?q=)?(?:${lang}/)?system/files(/[^>]*?>)!is";
    $replacement = '$1file://'. realpath(file_directory_path()) .'$2';
    $print['content'] = preg_replace($pattern, $replacement, $print['content']);
    $print['logo'] = preg_replace($pattern, $replacement, $print['logo']);
    $print['footer_message'] = preg_replace($pattern, $replacement, $print['footer_message']);
  }

  $node = $print['node'];
  ob_start();
  include_once(_print_get_template(PRINT_PDF_FORMAT, $print['type']));
  $html = ob_get_contents();
  ob_end_clean();

  // Convert the a href elements
  $pattern = '!<(a\s[^>]*?)>!is';
  $html = preg_replace_callback($pattern, '_print_rewrite_urls', $html);

  $pdf_filename = variable_get('print_pdf_filename', PRINT_PDF_FILENAME_DEFAULT);
  if (function_exists('token_replace') && !empty($pdf_filename)) {
    $pdf_filename = token_replace($pdf_filename, 'node', $node) .'.pdf';
  }
  else {
    $pdf_filename = str_replace('/', '_', $path) .'.pdf';
  }
  if (basename($print_pdf_pdf_tool) == 'dompdf_config.inc.php') {
    _print_pdf_dompdf($print, $html, $pdf_filename);
  }
  elseif (basename($print_pdf_pdf_tool) == 'tcpdf.php') {
    _print_pdf_tcpdf($print, $html, $pdf_filename);
  }
  else {
    return drupal_not_found();
  }

  $nodepath = drupal_get_normal_path($node->path);
  db_query("UPDATE {print_pdf_page_counter} SET totalcount = totalcount + 1, timestamp = %d WHERE path = '%s'", time(), $nodepath);
  // If we affected 0 rows, this is the first time viewing the node.
  if (!db_affected_rows()) {
    // We must create a new row to store counters for the new node.
    db_query("INSERT INTO {print_pdf_page_counter} (path, totalcount, timestamp) VALUES ('%s', 1, %d)", $nodepath, time());
  }
}

/**
 * Generate the PDF file using the dompdf library
 *
 * @param $print
 *   array containing the configured data
 * @param $html
 *   contents of the post-processed template already with the node data
 * @param $filename
 *   name of the PDF file to be generated
 * @see print_pdf_controller()
 */
function _print_pdf_dompdf($print, $html, $filename) {
  $print_pdf_pdf_tool = variable_get('print_pdf_pdf_tool', PRINT_PDF_PDF_TOOL_DEFAULT);
  $print_pdf_paper_size = variable_get('print_pdf_paper_size', PRINT_PDF_PAPER_SIZE_DEFAULT);
  $print_pdf_page_orientation = variable_get('print_pdf_page_orientation', PRINT_PDF_PAGE_ORIENTATION_DEFAULT);
  $print_pdf_content_disposition = variable_get('print_pdf_content_disposition', PRINT_PDF_CONTENT_DISPOSITION_DEFAULT);
  require_once($print_pdf_pdf_tool);

  // dompdf seems to have problems with something in system.css so let's not use it
  $html = preg_replace('!<link.*?modules/system/system.css.*?/>!', '', $html);

  $url_array  = parse_url($print['url']);

  $protocol = $url_array['scheme'] .'://';
  $host = $url_array['host'];
  $path = dirname($url_array['path']) .'/';

  $dompdf = new DOMPDF();
  $dompdf->set_base_path($path);
  $dompdf->set_host($host);
  $dompdf->set_paper(drupal_strtolower($print_pdf_paper_size), $print_pdf_page_orientation);
  $dompdf->set_protocol($protocol);

// dompdf can't handle footers cleanly, so disable the following
//  $html = theme('print_pdf_dompdf_footer', $html);

  // Convert from UTF-8 to ISO-8859-1 and then to HTML entities
  $all_entities = get_html_translation_table(HTML_ENTITIES, ENT_NOQUOTES);
  $special_entities = get_html_translation_table(HTML_SPECIALCHARS, ENT_NOQUOTES);
  $no_tags = array_diff($all_entities, $special_entities);
  $html = strtr(utf8_decode($html), $no_tags);

  //must get rid of tbody (dompdf goes into recursion)
  $html = preg_replace('!<tbody[^>]*?>|</tbody>!i', '', $html);

  $dompdf->load_html($html);

  $dompdf->render();
  $dompdf->stream($filename, array('Attachment' => ($print_pdf_content_disposition == 2)));
}

/**
 * Generate the PDF file using the TCPDF library
 *
 * @param $print
 *   array containing the configured data
 * @param $html
 *   contents of the post-processed template already with the node data
 * @param $filename
 *   name of the PDF file to be generated
 * @see print_pdf_controller()
 */
function _print_pdf_tcpdf($print, $html, $filename) {
  global $base_url, $language;

  $print_pdf_pdf_tool = variable_get('print_pdf_pdf_tool', PRINT_PDF_PDF_TOOL_DEFAULT);
  $print_pdf_paper_size = variable_get('print_pdf_paper_size', PRINT_PDF_PAPER_SIZE_DEFAULT);
  $print_pdf_page_orientation = variable_get('print_pdf_page_orientation', PRINT_PDF_PAGE_ORIENTATION_DEFAULT);
  $print_pdf_content_disposition = variable_get('print_pdf_content_disposition', PRINT_PDF_CONTENT_DISPOSITION_DEFAULT);

  $pdf_tool_path = realpath(dirname($print_pdf_pdf_tool));

  define('K_TCPDF_EXTERNAL_CONFIG', TRUE);
  define('K_PATH_MAIN', dirname($_SERVER['SCRIPT_FILENAME']));
  define('K_PATH_URL', $base_url);
  define('K_PATH_FONTS', $pdf_tool_path .'/fonts/');
  define('K_PATH_CACHE', $pdf_tool_path .'/cache/');
  define('K_PATH_IMAGES', '');
  define('K_BLANK_IMAGE', $pdf_tool_path .'/images/_blank.png');
  define('K_CELL_HEIGHT_RATIO', 1.25);
  define('K_SMALL_RATIO', 2/3);

  require_once($print_pdf_pdf_tool);
  if (strpos(PDF_PRODUCER, 'PHP4') === FALSE) {
    require_once(drupal_get_path('module', 'print_pdf') .'/print_pdf.class.inc');
  }
  else {
    require_once(drupal_get_path('module', 'print_pdf') .'/print_pdf.class_php4.inc');
  }

  $font = Array(
    variable_get('print_pdf_font_family', PRINT_PDF_FONT_FAMILY_DEFAULT),
    '',
    variable_get('print_pdf_font_size', PRINT_PDF_FONT_SIZE_DEFAULT),
  );
  $orientation = drupal_strtoupper($print_pdf_page_orientation[0]);

  // create new PDF document
  $pdf = new PrintTCPDF($orientation , 'mm', $print_pdf_paper_size, TRUE);

  // set document information
  $pdf->SetAuthor(strip_tags($print['submitted']));
  $pdf->SetCreator(variable_get('site_name', 'Drupal'));
  $pdf->SetTitle($print['title']);
  $keys = implode(' ', explode("\n", trim(strip_tags($print['taxonomy']))));
  $pdf->SetKeywords($keys);
  $pdf->setPDFVersion('1.6');

  if (defined('LANGUAGE_RTL') && $language->direction == LANGUAGE_RTL) {
    $pdf->setRTL(TRUE);
  }

  $pdf = theme('print_pdf_tcpdf_header', $pdf, $html, $font);
  $pdf = theme('print_pdf_tcpdf_footer', $pdf, $html, $font);
  $pdf = theme('print_pdf_tcpdf_page', $pdf);

  //initialize document
  $pdf->AliasNbPages();

  // add a page
  $pdf->AddPage();

  $pdf = theme('print_pdf_tcpdf_content', $pdf, $html, $font);

  // reset pointer to the last page
  $pdf->lastPage();

  //Close and output PDF document
  $output_dest = ($print_pdf_content_disposition == 2) ? 'D' : 'I';
  $pdf->Output($filename, $output_dest);
}

/**
 * Format the dompdf footer contents
 *
 * @param $html
 *   contents of the body of the HTML from the original node
 * @see theme_print_pdf_tcpdf_footer()
 */
function theme_print_pdf_dompdf_footer(&$html) {
  preg_match('!<div class="print-footer">(.*?)</div>!si', $html, $tpl_footer);
  $html = str_replace($tpl_footer[0], '', $html);

  $text = '<script type="text/php">
    if (isset($pdf)) {
      $font = Font_Metrics::get_font("verdana");;
      $size = 10;
      $color = array(0,0,0);
      $text_height = Font_Metrics::get_font_height($font, $size);

      $w = $pdf->get_width();
      $h = $pdf->get_height();

      $footer = $pdf->open_object();

      // Draw a line along the bottom
      $y = $h - 25;
      $pdf->line(15, $y, $w - 15, $y, $color, 1);

      $y += $text_height / 2;
      $pdf->page_text(15, $y, \''. addslashes(strip_tags($tpl_footer[1])) .'\', $font, $size, $color);

      $pdf->close_object();
      $pdf->add_object($footer, "all");

      // Center the text
      $width = Font_Metrics::get_text_width("Page 1 of 2", $font, $size);
      $pagenumtxt = t("Page !n of !total", array("!n" => "{PAGE_NUM}", "!total" => "{PAGE_COUNT}"));
      $pdf->page_text($w - 15 - $width, $y, $pagenumtxt, $font, $size, $color);
    }
  </script>';

  return str_replace("<body>", "<body>" . $text, $html);
}

/**
 * Format the TCPDF header
 *
 * @param $pdf
 *   current TCPDF object
 * @param $html
 *   contents of the body of the HTML from the original node
 * @param $font
 *   array with the font definition (font name, styles and size)
 * @see theme_print_pdf_tcpdf_header()
 */
function theme_print_pdf_tcpdf_header(&$pdf, &$html, $font) {
  preg_match('!<div class="print-logo">(.*?)</div>!si', $html, $tpl_logo);
  preg_match('!<h1 class="print-title">(.*?)</h1>!si', $html, $tpl_title);
  preg_match('!<div class="print-site_name">(.*?)</div>!si', $html, $tpl_site_name);

  $ratio = 0;
  $logo = '';
  $logo_ret = preg_match('!src\s*=\s*(\'.*?\'|".*?"|[^\s]*)!i', $tpl_logo[1], $matches);
  if ($logo_ret) {
    $logo = trim($matches[1], '\'"');
    $size = getimagesize($logo);
    $ratio = $size ? ($size[0] / $size[1]) : 0;
  }

  // set header font
  $pdf->setHeaderFont($font);
  // set header margin
  $pdf->SetHeaderMargin(5);
  // set header data
  $pdf->SetHeaderData($logo, 10 * $ratio, $tpl_title[1], strip_tags($tpl_site_name[1]));

  return $pdf;
}

/**
 * Format the TCPDF page settings (margins, etc)
 *
 * @param $pdf
 *   current TCPDF object
 * @see theme_print_pdf_tcpdf_page()
 */
function theme_print_pdf_tcpdf_page(&$pdf) {
  // set margins
  $pdf->SetMargins(15, 20, 15);
  // set auto page breaks
  $pdf->SetAutoPageBreak(TRUE, 15);
  // set image scale factor
  $pdf->setImageScale(4);
  // set image compression quality
  $pdf->setJPEGQuality(100);

  return $pdf;
}

/**
 * Format the TCPDF page content
 *
 * @param $pdf
 *   current TCPDF object
 * @param $html
 *   contents of the body of the HTML from the original node
 * @param $font
 *   array with the font definition (font name, styles and size)
 * @see theme_print_pdf_tcpdf_content()
 */
function theme_print_pdf_tcpdf_content(&$pdf, &$html, $font) {
  // set content font
  $pdf->setFont($font[0], $font[1], $font[2]);

  preg_match('!<body.*?>(.*)</body>!sim', $html, $matches);
  $pattern = '!(?:<div class="print-(?:logo|site_name|breadcrumb|footer)">.*?</div>|<hr class="print-hr" />)!si';
  $matches[1] = preg_replace($pattern, '', $matches[1]);

  // Since TCPDF's writeHTML is so bad with <p>, do everything possible to make it look nice
  $matches[1] = preg_replace('!(<p>|<p\s*/>|</p>)!i', '<br />', $matches[1]);
  $matches[1] = str_replace(array('<div', 'div>'), array('<span', 'span><br />'), $matches[1]);

  $pdf->writeHTML($matches[1]);

  return $pdf;
}

/**
 * Format the TCPDF footer contents
 *
 * @param $pdf
 *   current TCPDF object
 * @param $html
 *   contents of the body of the HTML from the original node
 * @param $font
 *   array with the font definition (font name, styles and size)
 * @see theme_print_pdf_tcpdf_footer()
 */
function theme_print_pdf_tcpdf_footer(&$pdf, &$html, $font) {
  preg_match('!<div class="print-footer">(.*?)</div>!si', $html, $tpl_footer);
  $footer = trim(preg_replace('!</?div[^>]*?>!i', '', $tpl_footer[1]));

  // set footer font
  $font[2] *= 0.8;
  $pdf->setFooterFont($font);
  // set footer margin
  $pdf->SetFooterMargin(10);
  // set footer data
  $pdf->SetFooterData($footer);

  return $pdf;
}

/**
 * Format the TCPDF footer layout
 *
 * @param $pdf
 *   current TCPDF object
 * @see theme_print_pdf_tcpdf_footer2()
 */
function theme_print_pdf_tcpdf_footer2(&$pdf) {
  //Position at 1.5 cm from bottom
  $pdf->writeHTMLCell(0, 15, 15, 0, $pdf->footer, 0, 0, 0, TRUE, '');

  $ormargins = $pdf->getOriginalMargins();
  $pagenumtxt = t('Page !n of !total', array('!n' => $pdf->PageNo(), '!total' => $pdf->getAliasNbPages()));
  //Print page number
  if ($pdf->getRTL()) {
    $pdf->SetX($ormargins['right']);
    $pdf->Cell(0, 10, $pagenumtxt, 'T', 0, 'L');
  }
  else {
    $pdf->SetX($ormargins['left']);
    $pdf->Cell(0, 10, $pagenumtxt, 'T', 0, 'R');
  }

  return $pdf;
}
