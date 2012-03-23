<?php
/* $Id */

function votingapi_rebuild_action_cache() {
  // This builds a live cache of all the currently enabled sets,
  // first from the database, then from module 'defaults'. Any
  // set saved in the DB is assumed to be enabled, and any DB
  // set overrides a module set with the same name.

  $sets = _votingapi_load_action_sets_from_db();
  $module_sets = _votingapi_load_action_sets_from_modules();
  
  foreach($sets as $name => $set) {
    $active_sets[$name] = $set;
  }

  $status_list = variable_get('votingapi_action_status', array());
  foreach($module_sets as $name => $set) {
    if (!in_array($name, array_keys($sets))) {
      if ($status_list[$name]) {
        $sets[$name] = $set;
      }
    }
  }
  cache_set("votingapi_action_sets", serialize($sets));
}

function _votingapi_get_action_status($set_name) {
  $status_list = variable_get('votingapi_action_status', array());
  return $status_list[$set_name];
}

function _votingapi_set_action_status($set_name, $status) {
  $status_list = variable_get('votingapi_action_status', array());
  $status_list[$set_name] = $status;
  variable_set('votingapi_action_status', $status_list);
  votingapi_rebuild_action_cache();
}

function _votingapi_load_action_sets_from_modules() {
  $sets = array();
  $status_list = variable_get('votingapi_action_status', array());

  $set_data = module_invoke_all('votingapi_action_sets');
  foreach ($set_data as $name => $set) {
    $sets[$name] = $set;
    if (!isset($status_list[$name])) {
      $status_list[$name] = $set['enabled'];
    }
  }
  
  variable_set('votingapi_action_status', $status_list);
  return $sets;
}

function _votingapi_load_action_sets_from_db($parent = '') {
  $sets = array();
  $result = db_query("SELECT * FROM {votingapi_action_set} WHERE parent_name = '%s' ORDER BY weight, name ASC", $parent);   

  while ($set = db_fetch_array($result)) {
    $set_name = $set['name'];
    unset($set['name']);

    $condition_result = db_query("SELECT * FROM {votingapi_action_condition} WHERE parent_name = '%s' ORDER BY weight ASC", $set_name);
    while ($condition = db_fetch_array($condition_result)) {
      $condition['data'] = unserialize($condition['data']);
      $name = $condition['name'];
      unset($condition['name']);
      $set['conditions'][$name] = $condition;
    }

    $action_result = db_query("SELECT * FROM {votingapi_action} WHERE parent_name = '%s' ORDER BY aid ASC", $set_name);
    while ($action = db_fetch_array($action_result)) {
      $set['actions'][] = $action['aid'];
    }

    $set->subsets = _votingapi_load_action_sets_from_db($set_name);
    $sets[$set_name] = $set;
  }
  
  return $sets;
}

function _votingapi_insert_set($name, $set) {
  $sql = "INSERT INTO {votingapi_action_set} ";
  $sql .= "(name, parent_name, content_type, source, description, condition_mask, required, weight)";
  $sql .= "VALUES ('%s', '%s', '%s', '%s', '%s', '%s', %d, %d)";
  
  db_query($sql, $name, $set['parent_name'], $set['content_type'],
    $set['source'], $set['description'], $set['condition_mask'], 
    $set['required'], $set['weight']);
  
  if (is_array($set['conditions'])) {
    foreach ($set['conditions'] as $cname => $condition) {
      $condition['parent_name'] = $name;
      _votingapi_insert_condition($cname, $condition);
    }
  }
  
  if (is_array($set['actions'])) {
    foreach ($set['actions'] as $action) {
      db_query("INSERT INTO {votingapi_action} (parent_name, aid) VALUES ('%s', '%s')", $name, $action);
    }
  }

  if (is_array($set['sets'])) {
    foreach ($set['sets'] as $sname => $subset) {
      $subset['parent_name'] = $name;
      _votingapi_insert_set($sname, $subset);
    }
  }
}

function _votingapi_insert_condition($name, $condition) {
  $sql = "INSERT INTO {votingapi_action_condition} ";
  $sql .= "(name, parent_name, description, data, handler, weight)";
  $sql .= "VALUES ('%s', '%s', '%s', '%s', '%s', %d)";

  db_query($sql, $name, $condition['parent_name'], $condition['description'],
    serialize($condition['data']), $condition['handler'], $condition['weight']);
}

function _votingapi_update_set($name, $old_name, $set) {
  $sql = "UPDATE {votingapi_action_set} SET ";
  $sql .= "name = '%s', parent_name = '%s', content_type = '%s', source = '%s', description = '%s', condition_mask = '%s', ";
  $sql .= "required = %d, weight = %d ";
  $sql .= "WHERE name = '%s'";
  
  db_query($sql, $name, $set['parent_name'], $set['content_type'],
    $set['source'], $set['description'], $set['condition_mask'], 
    $set['required'], $set['weight'], $old_name);

  db_query("DELETE FROM {votingapi_action_condition} WHERE parent_name = '%s'", $old_name);
  if (is_array($set['conditions'])) {
    foreach ($set['conditions'] as $cname => $condition) {
      $condition['parent_name'] = $name;
      _votingapi_insert_condition($cname, $condition);
    }
  }

  db_query("DELETE FROM {votingapi_action} WHERE parent_name = '%s'", $old_name);
  if (is_array($set['actions'])) {
    foreach ($set['actions'] as $action) {
      db_query("INSERT INTO {votingapi_action} (parent_name, aid) VALUES ('%s', '%s')", $name, $action);
    }
  }
  
  if (is_array($set['sets'])) {
    foreach ($set['sets'] as $sname => $subset) {
      $subset['parent_name'] = $name;
      if ($subset['new']) {
        _votingapi_insert_set($sname , $subset);
      }
      else {
        _votingapi_update_set($sname , $subset);
      }
    }
  }

  if (is_array($set['deleted_sets'])) {
    foreach ($set['deleted_sets'] as $sname => $subset) {
      $subset['parent_name'] = $name;
      _votingapi_delete_set($sname , $subset);
    }
  }
}

function _votingapi_update_condition($name, $condition) {
  $sql = "UPDATE {votingapi_action_condition} SET ";
  $sql .= "name = '%s', parent_name = '%s', description = '%s', data = '%s', handler = '%s', weight = %d ";
  $sql .= "WHERE name = '%s' AND parent_name = '%s'";
  
  db_query($sql, $name, $condition['parent_name'], $condition['description'],
    serialize($condition['data']), $condition['handler'], $condition['weight'], 
    $name, $condition['parent_name']);
}

function _votingapi_delete_set($name, $set) {
  if (is_array($set['sets'])) {
    foreach ($set['sets'] as $sname => $subset) {
      $subset['parent_name'] = $name;
      _votingapi_delete_set($sname, $subset);
    }
  }

  db_query("DELETE FROM {votingapi_action_condition} WHERE parent_name = '%s'", $name);
  db_query("DELETE FROM {votingapi_action} WHERE parent_name = '%s'", $name);
  db_query("DELETE FROM {votingapi_action_set} WHERE name = '%s'", $name);
}

function _votingapi_validate_action_set($name, $set) {
  $errors = array();

  if (!is_array($set)) {
    $errors[] = "The set '$name' is not an array!";
    return $errors;
  }
  
  if (!isset($set['content_type']) && !isset($set['parent_name'])) {
    $errors[] = "The set '$name' must have a content_type.";
  }

  if ($set['condition_mask'] != 'AND' && $set['condition_mask'] != 'OR') {
    $errors[] = "The set '$name' must define a condition_mask of 'AND' or 'OR'.";
  }

  if (!isset($set['conditions'])) {
    $errors[] = "The set '$name' has no conditions defined.";
  }
  else {
    if (count($set['conditions']) == 0) {
      $errors[] = "The set '$name' has no conditions defined.";
    }
  }

  if (is_array($set['conditions'])) {
    foreach ($set['conditions'] as $cname => $condition) {
      $errors = array_merge($errors, _votingapi_validate_action_condition($cname, $condition));
    }
  }

  if (is_array($set['sets'])) {
    foreach ($set['sets'] as $subname => $subset) {
      $errors = array_merge($errors, _votingapi_validate_action_set($subname, $subset));
    }
  }
  return $errors;
}

function _votingapi_validate_action_condition($name, $condition) {
  $errors = array();
  if (!is_array($condition)) {
    $errors[] = "The condition '$name' is not an array!";
    return $errors;
  }

  if (!isset($condition['handler'])) {
    $errors[] = "The condition '$name' has no handler.";
  }

  if (!function_exists($condition['handler'])) {
    $handler = $condition['handler'];
    $errors[] = "The condition '$name' has an invalid handler ($handler).";
  }

  return $errors;
}

/**
 * Functions that integrate VotingAPI with the Actions module.
 * Allows VotingAPI-based modules to insert nested sets of conditionals
 * and actions to be executed whenever a voting result is fired off.
 */


// Called by the Voting API whenever a result is calculated.
// Other helper functions build the actions cache from the database.

function _votingapi_process_actions($content_id, $content_type, $votes, $results) {
  $data = cache_get('votingapi_action_sets');
  $action_sets = unserialize($data->data);

  if (!is_array($action_sets)) {
    return;
  }

  $content = _votingapi_load_content($content_id, $content_type);
  if ($content == NULL) {
    return;
  }

  foreach ($action_sets as $action_set) {
    if ($action_set['content_type'] == $content_type) {
      $actions = array();
      _votingapi_process_action_set($content, $votes, $results, $action_set, $actions);
      foreach ($actions as $action) {
        actions_do($action, $content);
      }
    }
  }
}

// An internal utility function that calls itself recursively to evaluate a
// tree of voting action sets. $actions is passed in by references, and accumulates
// actions-to-initiate. The calling function is responsible for firing them off.

function _votingapi_process_action_set($content = NULL, $votes = array(), $results = array(), $action_set = NULL, &$actions) {
  // a little safety code to catch malformed sets.
  if (!isset($action_set['conditions'])) {
    $action_set['conditions'] = array();
  }
  if (!isset($action_set['actions'])) {
    $action_set['actions'] = array();
  }
  if (!isset($action_set['subsets'])) {
    $action_set['subsets'] = array();
  }

  // Here, we iterate through every rule. The value starts as true,
  // and a single false will trip it to failure state.
  foreach($action_set['conditions'] as $condition) {
    $function = $condition['handler'];
    if (function_exists($function)) {
      // this calls a handler with several ops. 'process' and 'input' are the two i've thought of.
      $conditions_result = $function('process', $content, $votes, $results, $condition);
    }
    else {
      $conditions_result = FALSE;
    }
    if ($action_set['condition_mask'] == 'AND') {
      if ($conditions_result === FALSE) {
        // bail out to avoid unecessary processing.
        return FALSE;
      }
      else {
        // AND the set result and rule result together.
        if (isset($set_result)) {
          $set_result = $set_result && $conditions_result;
        }
        else {
          $set_result = $conditions_result;
        }
      }
    }
    else if ($action_set['condition_mask'] == 'OR') {
      // OR the set result and rule result together.
      $set_result = $set_result || $conditions_result;
    }
  }
 
  if ($set_result == TRUE) {
    // Now check sub-actions.

    foreach($action_set['subsets'] as $subset) {
      // check the required flag of the subset. if it is, evaluate it.
      if ($subset['required'] == TRUE) {
        $set_result = $set_result && _votingapi_process_action_set($content, $votes, $results, $subset, $actions);
        if ($set_result == FALSE) {
          return FALSE;
        }
      }
    }

    if ($set_result == TRUE) {
      // It's still true after executing required subsets. Add the actions, then process optional subsets.
      foreach ($action_set['actions'] as $action) {
        $actions[] = $action;
      }
      
      foreach($action_set['subsets'] as $subset) {
        // now handle the non-required subsets
        if ($subset['required'] == FALSE) {
          _votingapi_process_action_set($content, $votes, $results, $subset, $actions);
        }
      }
    }
  }

  return $set_result;
}

/*********************************************
 *  VOTINGAPI ACTION CONDITION HANDLERS
 *  TO BE USED BY OTHER MODULES AND USERS
 *********************************************/


function votingapi_vote_result_handler($op, $content, $votes, $results, $rule) {
  if ($op == 'process') {
    //  for this handler, $rule->data is an array in the following format:
    //
    //    $value = array(
    //      'value_type' => 'percent',   // an array of 1-n value types.
    //      'tag' => 'vote',             // an array of 1-n tags
    //      'comparison' = '<',                  // the comparison operator
    //      'value' => '90',                     // the value to be compared
    //      ),
    //    );
    //
    //  In the example above, any aggregate vote result in which a piece of content receives an 
    //  average percentage vote between 75% and 90% would match. Obviously, the specific values
    //  will change based on the specific action. If one of the above values is NOT specified
    //  it will be skipped. 
  
    $data = (object)$rule->data;
    $passed = FALSE;
    
    // loop through all the result objects and see if there's one that satisfies all the conditions.
    foreach ($results as $result) {
      if (isset($data->value_type)) {
        if ($result->value_type != $data->value_type) {
          continue;
        }
      }
  
      if (isset($data->tag)) {
        if ($result->tag != $data->tag) {
          continue;
        }
      }
      
      if (isset($data->function)) {
        if ($result->function != $data->function) {
          continue;
        }
      }

      switch ($data->comparison) {
        case '<' :
          if (!($result->value < $data->value)) {
            continue;
          }
          break;

        case '<=' :
          if (!($result->value <= $data->value)) {
            continue;
          }
          break;

        case '==' :
          if (!($result->value == $data->value)) {
            continue;
          }
          break;

        case '!=' :
          if (!($result->value != $data->value)) {
            continue;
          }
          break;

        case '>=' :
          if (!($result->value >= $data->value)) {
            continue;
          }

          break;

        case '>' :
          if (!($result->value > $data->value)) {
            continue;
          }
          break;
      }

      // if we get this far, one of the result records has passed successfully.
      $passed = TRUE;
      break;
    }

    return $passed;
  }
  else if ($op == 'form') {
    $form['value_type'] = array(
      '#type' => 'select',
      '#options' => votingapi_cache_value_types(),
    );
  
    $form['tag'] = array(
      '#type' => 'select',
      '#options' => votingapi_cache_tags(),
    );
  
    $form['function'] = array(
      '#type' => 'select',
      '#options' => votingapi_cache_functions(),
    );
    $form['comparison'] = array(
      '#type' => 'select',
      '#options' => array('==' => 'Is', '!=' => 'Is not', '<' => 'Is less than', '>' => 'Is greater than'),
    );
    $form['value'] = array(
      '#type' => 'textfield',
      '#maxlength' => 10,
    );
    
    return $form;
  }
  else if ($op == 'name') {
    return array('votingapi_vote_result_handler' => 'Vote result properties');
  }
}


function votingapi_vote_handler($op, $content, $votes, $results, $rule) {
  if ($op == 'process') {
    //  for this handler, $rule->data is an array in the following format:
    //
    //    $value = array(
    //      'value_type' => 'percent',   // an array of 1-n value types.
    //      'tag' => 'vote',             // an array of 1-n tags
    //      'function' => 'average',             // a single aggregate function
    //      'comparison' = '<',                  // the comparison operator
    //      'value' => '90',                     // the value to be compared
    //      ),
    //    );
    //
    //  In the example above, any aggregate vote result in which a piece of content receives an 
    //  average percentage vote between 75% and 90% would match. Obviously, the specific values
    //  will change based on the specific action. If one of the above values is NOT specified
    //  it will be skipped. 
  
    $data = (object)$rule->data;
    $passed = FALSE;
    
    // loop through all the result objects and see if there's one that satisfies all the conditions.
    foreach ($results as $result) {
      if (isset($data->value_type)) {
        if ($result->value_type != $data->value_type) {
          continue;
        }
      }
  
      if (isset($data->tag)) {
        if ($result->tag != $data->tag) {
          continue;
        }
      }

      switch ($data->comparison) {
        case '<' :
          if (!($result->value < $data->value)) {
            continue;
          }
          break;

        case '<=' :
          if (!($result->value <= $data->value)) {
            continue;
          }
          break;

        case '==' :
          if (!($result->value == $data->value)) {
            continue;
          }
          break;

        case '!=' :
          if (!($result->value != $data->value)) {
            continue;
          }
          break;

        case '>=' :
          if (!($result->value >= $data->value)) {
            continue;
          }

          break;

        case '>' :
          if (!($result->value > $data->value)) {
            continue;
          }
          break;
      }

      // if we get this far, one of the result records has passed successfully.
      $passed = TRUE;
      break;
    }
    
    return $passed;
  }
  else if ($op == 'form') {
    $form['value_type'] = array(
      '#type' => 'select',
      '#options' => votingapi_vote_value_types(),
    );
  
    $form['tag'] = array(
      '#type' => 'select',
      '#options' => votingapi_vote_tags(),
    );
  
    $form['comparison'] = array(
      '#type' => 'select',
      '#options' => array('==' => 'Is', '!=' => 'Is not', '<' => 'Is less than', '>' => 'Is greater than'),
    );
    $form['value'] = array(
      '#type' => 'textfield',
      '#maxlength' => 10,
    );
    
    return $form;
  }
  else if ($op == 'name') {
    return array('votingapi_vote_handler' => 'Individual vote properties');
  }
}

function votingapi_node_properties_handler($op, $content = NULL, $votes = NULL, $results = NULL, $rule = NULL) {
  if ($op == 'process') {
    //  for this handler, $rule->data is a keyed array of comaprisons by node property name:
    //
    //  $rule->data = array(
    //    'status' => array('==' => 1),   // must be published
    //    'uid' => array('!=' => 1),      // not authored by the admin account
    //  );
    //
    //  The keys in the sub-array are comparison operators, and the values are the value to
    //  compare to. This is mainly useful for ensuring that a node hasn't yet been promoted
    //  before promoting it, etc. At present only == and != are supported by this handler.
  
    $property = $rule['data']['property'];
    $comparison = $rule['data']['comparison'];
    $value = $rule['data']['value'];;

    switch ($comparison) {
      case '==' :
        if (!($content->$property == $value)) {
          return FALSE;
        }
        break;

      case '!=' :
        if (!($content->$property != $value)) {
          return FALSE;
        }
        break;
    }

    return TRUE;
  }
  else if ($op == 'form') {
    $form['property'] = array(
      '#type' => 'textfield',
      '#maxlength' => 10,
    );
    $form['comparison'] = array(
      '#type' => 'select',
      '#options' => array('==' => 'Is', '!=' => 'Is not'),
    );
    $form['value'] = array(
      '#type' => 'textfield',
      '#maxlength' => 10,
    );
    
    return $form;
  }
  else if ($op == 'name') {
    return array('votingapi_node_properties_handler' => 'Node properties');
  }
}

/*********************************************
 *  VOTINGAPI IMPLEMENTED ACTIONS. SHOULD
 *  PROBABLY BE ADDED TO ACTIONS.MODULE
 *********************************************/

/**
 * Touches the creation date of a node. Useful for moderated nodes that should appear
 * 'fresh' as soon as they're promoted.
 */
function action_node_touch_created($op, $edit = array(), &$node) {
  switch($op) {
    case 'do':
      $node->created = time();
      if (!$edit['defer']) {
        node_save($node);
      }
      watchdog('action', t('Touched creation date of node id %id', array('%id' => intval($node->nid))));
      break;

    case 'metadata':
      return array(
        'description' => t('Touch node creation date'),
        'type' => t('Node'),
        'batchable' => true,
        'configurable' => false,
      );

    // return an HTML config form for the action
    case 'form':
      return '';

    // validate the HTML form
    case 'validate':
      return TRUE;

    // process the HTML form to store configuration
    case 'submit':
      return '';
  }
}

/**
 * Touches the change date of a node. Useful for moderated nodes that should appear
 * 'fresh' as soon as they're promoted.
 */
function action_node_touch_changed($op, $edit = array(), &$node) {
  switch($op) {
    case 'do':
      $node->changed = time();
      if (!$edit['defer']) {
        node_save($node);
      }
      watchdog('action', t('Touched change date of node id %id', array('%id' => intval($node->nid))));
      break;

    case 'metadata':
      return array(
        'description' => t('Touch node change date'),
        'type' => t('Node'),
        'batchable' => true,
        'configurable' => false,
      );

    // return an HTML config form for the action
    case 'form':
      return '';

    // validate the HTML form
    case 'validate':
      return TRUE;

    // process the HTML form to store configuration
    case 'submit':
      return '';
  }
}

/**
 * Sets the status of a comment to PUBLISHED.
 */
function action_comment_publish($op, $edit = array(), &$comment) {
  switch($op) {
    case 'do':
      $comment->status = COMMENT_PUBLISHED;
      comment_save((array)$comment);
      watchdog('action', t('Set comment id %id to Published', array('%id' => intval($comment->cid))));
      break;

    case 'metadata':
      return array(
        'description' => t('Publish comment'),
        'type' => t('Comment'),
        'batchable' => true,
        'configurable' => false,
      );

    // return an HTML config form for the action
    case 'form':
      return '';

    // validate the HTML form
    case 'validate':
      return TRUE;

    // process the HTML form to store configuration
    case 'submit':
      return '';
  }
}

function action_comment_unpublish($op, $edit = array(), &$comment) {
  switch($op) {
    case 'do':
      $comment->status = COMMENT_NOT_PUBLISHED;
      comment_save((array)$comment);
      watchdog('action', t('Set comment id %id to Unpublished', array('%id' => intval($comment->cid))));
      break;

    case 'metadata':
      return array(
        'description' => t('Unpublish comment'),
        'type' => t('Comment'),
        'batchable' => true,
        'configurable' => false,
      );

    // return an HTML config form for the action
    case 'form':
      return '';

    // validate the HTML form
    case 'validate':
      return TRUE;

    // process the HTML form to store configuration
    case 'submit':
      return '';
  }
}
