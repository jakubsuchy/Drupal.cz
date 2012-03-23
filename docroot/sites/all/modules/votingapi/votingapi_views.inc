<?php
// $Id: votingapi_views.inc,v 1.11.2.5 2007/03/22 12:50:30 eaton Exp $

function votingapi_views_tables() {
  $vote_tags = _votingapi_views_values($field = 'tag', $table = 'vote');
  $vote_types = _votingapi_views_values($field = 'value_type', $table = 'vote');

  $vote_tags = array_merge($vote_tags, array('vote'));
  $vote_types = array_merge($vote_types, array('percent'));

  foreach ($vote_tags as $vote_tag) {
    foreach ($vote_types as $vote_type) {
      $table = array(
        "name" => "votingapi_vote", 
        "provider" => "votingapi",
        "join" => array(
          "left" => array(
            "table" => "node",
            "field" => "nid"
          ), 
          "right" => array(
            "field" => "content_id"
          ),
          "extra" => array(
            'content_type' => 'node',
            'value_type' => $vote_type,
            'tag' => $vote_tag,
          ),
        ),
        "fields" => array(
          "value" => array(
            'name' => t("VotingAPI !type !tag value", array('!tag' => $vote_tag, '!type' => $vote_type)),
            'handler' => votingapi_get_formatters(array('value_type' => $vote_type, 'tag' => $vote_tag)),
            'value_type' => $vote_type,
            'tag' => $vote_tag,
            'sortable' => TRUE,
          ),
          "currentuservalue" => array(
            'name' => t("VotingAPI !type !tag value (current user only)", array('!tag' => $vote_tag, '!type' => $vote_type)),
            'field' => 'value',
            'handler' => votingapi_get_formatters(array('value_type' => $vote_type, 'tag' => $vote_tag)),
            'value_type' => $vote_type,
            'tag' => $tag,
            'query handler' => 'votingapi_views_qhandler_currentuser_vote',
            'sortable' => TRUE,
          ),

          "timestamp" => array(
            'name' => t("VotingAPI !type !tag timestamp", array('!tag' => $vote_tag, '!type' => $vote_type)),
            'sortable' => TRUE,
            'handler' => 
              array(
                "views_handler_field_date_small" => t('As Short Date'), 
                "views_handler_field_date" => t('As Medium Date'), 
                "views_handler_field_date_large" => t('As Long Date'), 
                "views_handler_field_since" =>  t('As Time Ago')
              ),
            'sortable' => TRUE,
            'help' => "Display the time the vote was cast.",
          ),
          "uid" => array(
            'name' => t("VotingAPI !type !tag user", array('!tag' => $vote_tag, '!type' => $vote_type)),
            'handler' => 'votingapi_views_handler_field_username', 
            'sortable' => TRUE, 
          ),
        ),  
        "sorts" => array(
          "value" => array('name' => t("VotingAPI !type !tag vote", array('!tag' => $vote_tag, '!type' => $vote_type))),
          "uid" => array('name' => t("VotingAPI !type !tag voter", array('!tag' => $vote_tag, '!type' => $vote_type))),
          "timestamp" => array('name' => t("VotingAPI !type !tag timestamp", array('!tag' => $vote_tag, '!type' => $vote_type))),
        ),
        'filters' => array(
          'value' => array(
            'name' => t("VotingAPI !type !tag value", array('!tag' => $vote_tag, '!type' => $vote_type)),
            'operator' => 'views_handler_operator_gtlt',
            'handler' => 'votingapi_handler_filter_value',
            'value-type' => 'string',
            'help' => t('Filter nodes by values of the individual votes users cast for them.'),
          ),
          'timestamp' => array(
            'name' => t("VotingAPI !type !tag timestamp", array('!tag' => $vote_tag, '!type' => $vote_type)),
            'operator' => 'views_handler_operator_gtlt',
            'value' => views_handler_filter_date_value_form(),
            'handler' => 'views_handler_filter_timestamp',
            'option' => 'string',
            'help' => t('Filter nodes by the date they were voted on.')
              .' '. views_t_strings('filter date'),
          ),
          'uid' => array(
            'name' => t("VotingAPI !type !tag user", array('!tag' => $vote_tag, '!type' => $vote_type)),
            'operator' => array('IS NOT NULL' => t('Has been voted on by'), 'IS NULL' => t('Has not been voted on by')),
            'list' => array('***CURRENT_USER***' => t('Currently Logged In User'), '***ANY_USER***' => t('Any user')),
            'list-type' => 'select',
            'handler' => 'votingapi_handler_filter_uid_voted',
            'help' => t('Filter nodes by whether the currently logged in user has voted.'),
            'content_type' => 'node',
            'value_type' => $vote_type,
            'tag' => $vote_tag,
          )
        ),
      );
      $tables['votingapi_vote_'. $vote_tag .'_'. $vote_type] = $table;
    }
  }

  $cache_tags = _votingapi_views_values($field = 'tag', $table = 'cache');
  $cache_types = _votingapi_views_values($field = 'value_type', $table = 'cache');
  $cache_functions = _votingapi_views_values($field = 'function', $table = 'cache');

  $cache_tags = array_merge($vote_tags, array('vote'));
  $cache_types = array_merge($cache_types, array('percent'));
  $cache_functions = array_merge($cache_functions, array('average', 'count'));

  foreach ($cache_functions as $cache_function) {
    foreach ($cache_tags as $cache_tag) {
      foreach ($cache_types as $cache_type) {
        $table = array(
          "name" => "votingapi_cache", 
          "provider" => "votingapi",
          "join" => array(
            "left" => array(
              "table" => "node",
              "field" => "nid"
            ), 
            "right" => array(
              "field" => "content_id"
            ),
            "extra" => array(
              'content_type' => 'node',
              'value_type' => $cache_type,
              'tag' => $cache_tag,
              'function' => $cache_function,
            ),
          ),
          "fields" => array(
            "value" => array(
              'name' => t("VotingAPI !type !tag result (!function)", array('!tag' => $cache_tag, '!type' => $cache_type, '!function' => $cache_function)),
              'handler' => votingapi_get_formatters(array('value_type' => $cache_type, 'tag' => $cache_tag, 'function' => $cache_function)),
              'value_type' => $cache_type,
              'tag' => $cache_tag,
              'function' => $cache_function,
              'sortable' => TRUE,
            ),
          ),  
          "sorts" => array(
            "value" => array('name' => t("VotingAPI !type !tag result (!function)", array('!tag' => $cache_tag, '!type' => $cache_type, '!function' => $cache_function))),
          ),
          'filters' => array(
            'value' => array(
              'name' => t("VotingAPI !type !tag result (!function)", array('!tag' => $cache_tag, '!type' => $cache_type, '!function' => $cache_function)),
              'operator' => 'views_handler_operator_gtlt',
              'handler' => 'votingapi_handler_filter_value',
              'value-type' => 'string',
              'help' => t('Filter nodes by the aggregate results of votes cast.'),
            ),
          ),
        );
        $tables[implode('_', array('votingapi_cache', $cache_tag, $cache_type, $cache_function))] = $table;
      }
    }
  }

  return $tables;
}

function votingapi_views_handler_field_username($fieldinfo, $fielddata, $value, $data) {
  if ($value === NULL) {
    return '';
  }
  else {
    $user = user_load(array('uid' => $value));
    return theme('username', $user);
  }
}

function votingapi_handler_filter_uid_voted($op, $filter, $filterinfo, &$query) {
  $table_data = _views_get_tables();
  $joininfo['left'] = array(
    'table' => 'node',
    'field' => 'nid'
  );
  $joininfo['right'] = array(
    'field' => 'content_id'
  );
  $joininfo['extra'] = array(
    'value_type' => $filterinfo['value_type'],
    'content_type' => $filterinfo['content_type'],
    'tag' => $filterinfo['tag'],
  );

  if ($filter['value'] != '***ANY_USER***') {
    $joininfo['extra']['uid'] = $filter['value'];
  }

  $tblnum = $query->add_table('votingapi_vote', false, 1, $joininfo);
  $table = $query->get_table_name('votingapi_vote', $tblnum);

  $query->add_where("$table.value %s", $filter['operator']);
}

function votingapi_views_qhandler_currentuser_vote($field, $fieldinfo, &$query) {
  $table = $fieldinfo['table'];
  $query->add_where("$table.uid = ***CURRENT_USER***");
}

function votingapi_handler_filter_value($op, $filter, $filterinfo, &$query) {
  $table = $filterinfo['table'];
  $column = $filterinfo['field'];
  $field = "$table.$column";
  $query->ensure_table($table);
  $query->add_where("%s %s %d", $field, $filter['operator'], $filter['value']);
}

function votingapi_get_formatters($details = array()) {
  $formatters = array(
    'votingapi_views_formatter_raw' => t('Raw value'),
    'votingapi_views_formatter_cleaned' => t('Cleaned version'),
  );
  $formatters = array_merge($formatters, module_invoke_all('votingapi_views_formatters', $details));
  return $formatters;
}

function votingapi_views_formatter_raw($fieldinfo, $fielddata, $value, $data) {
  return $value;
}


function votingapi_views_formatter_cleaned($fieldinfo, $fielddata, $value, $data) {
  if ($value === NULL) {
    return '';
  }

  switch ($fieldinfo['value_type']) {
    case 'percent':
      return round($value, 1) .'%';
    case 'points':
      return round($value, 1) .' '. t('points');
    default:
      return $value;
  }
}

function _votingapi_views_values($field = 'tag', $table = 'vote') {
  static $cached;
  if (!isset($cached[$table][$field])) {
    $results = db_query("SELECT DISTINCT %s FROM {votingapi_%s} WHERE content_type = 'node'", $field, $table);
    while ($result = db_fetch_object($results)) {
      $cached[$table][$field][$result->$field] = $result->$field;
    }
  }
  return empty($cached[$table][$field]) ? array() : $cached[$table][$field];
}