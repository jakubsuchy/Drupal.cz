<?php

/**
 * Implementation of hook_ctools_plugin_api().
 */
function dcz_catalog_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => 1);
  }
}

/**
 * Implementation of hook_node_info().
 */
function dcz_catalog_node_info() {
  $items = array(
    'content_katalog' => array(
      'name' => t('Záznam v katalogu webů'),
      'module' => 'features',
      'description' => t('Zde můžete přidat záznam v katalogu webů, které běží na Drupalu. Po přidání bude odkaz nejprve schválen administrátorem.'),
      'has_title' => '1',
      'title_label' => t('Jméno webu'),
      'has_body' => '0',
      'body_label' => '',
      'min_word_count' => '0',
      'help' => t('Věnujte pozornost celému formuláři pro zadání katalogového záznamu. Čím lépe ho vyplníte, tím lépe bude katalogový list vypadat a tim lepší bude Vaše vizitka. Nezapomeňte na popis webu, který je důležitou součástí, bez popisu nebude záznam zveřejněn. Všechna pole jsou povinná, včetně screenshotu. Dovolujeme si vyhradit právo nezveřejnit jakýkoliv záznam, důvod Vám však bude sdělen.
<b>Nepřidávejte weby, jejichž nejste autory!</b>'),
    ),
  );
  return $items;
}

/**
 * Implementation of hook_views_api().
 */
function dcz_catalog_views_api() {
  return array(
    'api' => '2',
  );
}
