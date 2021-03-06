<?php

/**
 * @file
 *   Entity group definitions and callbacks for the Activity Log module.
 */

/**
 * Defines the default entity groups.
 */
function activity_log_entity_groups($stream_owner = TRUE) {
  $og = module_exists('og');
  $ur = module_exists('user_relationships_api');
  $co = module_exists('comments');
  $fc = module_exists('fbss_comments');
  $ft = module_exists('facebook_status_tags');
  $tx = module_exists('taxonomy');
  $groups = array(
    'custom' => array(
      'items callback' => 'activity_log_eg_custom',
      'title' => t('Custom'),
      'expose fields' => array('id', 'type'),
    ),
    'acting user' => array(
      'items callback' => 'activity_log_eg_acting_user',
      'title' => t('Acting user'),
      'expose fields' => array('acting_uid'),
    ),
    'target entity' => array(
      'items callback' => 'activity_log_eg_target_entity',
      'title' => t('The entity on which the action was performed'),
      'additional arguments' => array($stream_owner),
    ),
  );
  if ($og) {
    $groups['active groups'] = array(
      'items callback' => 'activity_log_eg_active_groups',
      'title' => t('Active groups'),
    );
    $groups['active groups members'] = array(
      'items callback' => 'activity_log_eg_active_groups_members',
      'title' => t('Active group members'),
    );
  }
  if ($ur) {
    $groups['acting user relationships'] = array(
      'items callback' => 'activity_log_eg_acting_user_rels',
      'title' => t("The acting user's relationships"),
      'expose fields' => array('acting_uid'),
    );
    $groups['custom user relationships'] = array(
      'items callback' => 'activity_log_eg_custom_user_rels',
      'title' => t("A custom user's relationships"),
      'expose fields' => array('id'),
    );
    $ur_types = user_relationships_types_load();
    foreach ($ur_types as $type) {
      $groups['acting user relationships '. $type->rtid] = array(
        'items callback' => 'activity_log_eg_acting_user_rels_type',
        'title' => t("The acting user's %type relationships", array('%type' => $type->name)),
        'expose fields' => array('acting_uid'),
        'additional arguments' => array($type),
      );
      $groups['custom user relationships '. $type->rtid] = array(
        'items callback' => 'activity_log_eg_custom_user_rels_type',
        'title' => t("A custom user's %type relationships", array('%type' => $type->name)),
        'expose fields' => array('id'),
        'additional arguments' => array($type),
      );
    }
  }
  if ($og && $ur) {
    $groups['active groups relationships acting user'] = array(
      'items callback' => 'activity_log_eg_active_groups_rels_acting_user',
      'title' => t('Members of active groups who have a relationship with the acting user'),
      'expose fields' => array('acting_uid'),
    );
    $groups['active groups relationships custom user'] = array(
      'items callback' => 'activity_log_eg_active_groups_rels_custom_user',
      'title' => t('Members of active groups who have a relationship with a custom user'),
      'expose fields' => array('id'),
    );
    foreach ($ur_types as $type) {
      $groups['active groups relationships '. $type->rtid .' acting user'] = array(
        'items callback' => 'activity_log_eg_active_groups_rels_type_acting_user',
        'title' => t('Members of active groups who have a %type relationship with the acting user', array('%type' => $type->name)),
        'expose fields' => array('acting_uid'),
        'additional arguments' => array($type),
      );
      $groups['active groups relationships '. $type->rtid .' custom user'] = array(
        'items callback' => 'activity_log_eg_active_groups_rels_type_custom_user',
        'title' => t('Members of active groups who have a %type relationship with a custom user', array('%type' => $type->name)),
        'expose fields' => array('id'),
        'additional arguments' => array($type),
      );
    }
  }
  if ($co) {
    $groups['node commenters'] = array(
      'items callback' => 'activity_log_eg_node_commenters',
      'title' => t('Other people who have commented on this node (including the node author)'),
    );
  }
  if ($fc) {
    $groups['status commenters'] = array(
      'items callback' => 'activity_log_eg_status_commenters',
      'title' => t('Other people who have commented on this status (including the sender and recipient)'),
    );
  }
  if ($ft) {
    $groups['mentioned users'] = array(
      'items callback' => 'activity_log_eg_mentioned_users',
      'title' => t('The users @mentioned in a status'),
    );
    $groups['hashtag terms'] = array(
      'items callback' => 'activity_log_eg_hashtag_terms',
      'title' => t('The taxonomy terms corresponding to the #hashtags in a status'),
    );
  }
  if ($tx) {
    $groups['node terms'] = array(
      'items callback' => 'activity_log_eg_node_terms',
      'title' => t('The taxonomy terms used in a node'),
    );
  }
  if (!$stream_owner) {
    unset($groups['active groups'], $groups['hashtag terms'], $groups['node terms']);
    $groups['target entity']['title'] = t('The user on which the action was performed, if applicable');
    $groups['everyone'] = array(
      'items callback' => 'activity_log_eg_everyone',
      'title' => t('Everyone'),
    );
  }
  return $groups;
}

/**
 * Returns the entitity IDs for the Custom group.
 */
function activity_log_eg_custom($event, $settings, $id, $type) {
  return array($type => array($id));
}

/**
 * Returns the entitity IDs for the Acting user group.
 */
function activity_log_eg_acting_user($event, $settings, $acting_uid) {
  return array('user' => array($acting_uid));
}

/**
 * Returns the entitity IDs for the Target entity group.
 */
function activity_log_eg_target_entity($event, $settings, $stream_owner) {
  if (!$stream_owner || $event->target_type == 'user') {
    return array($event->target_type => array($event->target_id));
  }
}

/**
 * Returns the entitity IDs for the Active groups group.
 */
function activity_log_eg_active_groups($event, $settings) {
  if (module_exists('og')) {
    // Also look at og_get_node_groups($node).
    return array('node' => array(og_get_group_context()->nid));
  }
}

/**
 * Returns the entitity IDs for the Active groups' members group.
 */
function activity_log_eg_active_groups_members($event, $settings) {
  if (module_exists('og')) {
    return array('user' => _activity_log_get_active_group_members());
  }
}

/**
 * Returns the entitity IDs for the Acting user's relationships group.
 */
function activity_log_eg_acting_user_rels($event, $settings, $acting_uid) {
  if (module_exists('user_relationships_api')) {
    return array('user' => _activity_log_get_user_relationships($acting_uid));
  }
}

/**
 * Returns the entitity IDs for the Custom user's relationships group.
 */
function activity_log_eg_custom_user_rels($event, $settings, $id) {
  if (module_exists('user_relationships_api')) {
    return array('user' => _activity_log_get_user_relationships($id));
  }
}

/**
 * Returns the entitity IDs for the Acting user's %type relationships group.
 */
function activity_log_eg_acting_user_rels_type($event, $settings, $ur_type, $acting_uid) {
  if (module_exists('user_relationships_api')) {
    return array('user' => _activity_log_get_user_relationships($acting_uid, $ur_type->rtid));
  }
}

/**
 * Returns the entitity IDs for the Custom user's %type relationships group.
 */
function activity_log_eg_custom_user_rels_type($event, $settings, $ur_type, $id) {
  if (module_exists('user_relationships_api')) {
    return array('user' => _activity_log_get_user_relationships($id, $ur_type->rtid));
  }
}

/**
 * Returns the entitity IDs for the members of the active groups who have a
 * relationship with the acting user.
 */
function activity_log_eg_active_groups_rels_acting_user($event, $settings, $acting_uid) {
  if (module_exists('user_relationships_api') && module_exists('og')) {
    $members = _activity_log_get_active_group_members();
    $rels = _activity_log_get_user_relationships($acting_uid);
    return array('user' => array_intersect($members, $rels));
  }
}

/**
 * Returns the entitity IDs for the members of the active groups who have a
 * relationship with a custom user.
 */
function activity_log_eg_active_groups_rels_custom_user($event, $settings, $id) {
  if (module_exists('user_relationships_api') && module_exists('og')) {
    $members = _activity_log_get_active_group_members();
    $rels = _activity_log_get_user_relationships($id);
    return array('user' => array_intersect($members, $rels));
  }
}

/**
 * Returns the entitity IDs for the members of the active groups who have a
 * %type relationship with the acting user.
 */
function activity_log_eg_active_groups_rels_type_acting_user($event, $settings, $ur_type, $acting_uid) {
  if (module_exists('user_relationships_api') && module_exists('og')) {
    $members = _activity_log_get_active_group_members();
    $rels = _activity_log_get_user_relationships($acting_uid, $ur_type->rtid);
    return array('user' => array_intersect($members, $rels));
  }
}

/**
 * Returns the entitity IDs for the members of the active groups who have a
 * %type relationship with a custom user.
 */
function activity_log_eg_active_groups_rels_type_custom_user($event, $settings, $ur_type, $id) {
  if (module_exists('user_relationships_api') && module_exists('og')) {
    $members = _activity_log_get_active_group_members();
    $rels = _activity_log_get_user_relationships($id, $ur_type->rtid);
    return array('user' => array_intersect($members, $rels));
  }
}

/**
 * Returns the entitity IDs for the Node commenters group.
 */
function activity_log_eg_node_commenters($event, $settings) {
  if ($event->target_type == 'node' && module_exists('comment')) {
    $author = db_result(db_query("SELECT uid FROM {node} WHERE uid <> 0 AND nid = %d", $event->target_id));
    $uids = $author ? array($author) : array();
    $result = db_query("SELECT uid FROM {comments} WHERE uid <> 0 AND nid = %d", $event->target_id);
    while ($account = db_fetch_object($result)) {
      $uids[] = $account->uid;
    }
    return array('user' => $uids);
  }
}

/**
 * Returns the entitity IDs for the Status commenters group.
 */
function activity_log_eg_status_commenters($event, $settings) {
  if ($event->target_type == 'facebook_status' && module_exists('facebook_status')) {
    $status = db_fetch_object(db_query("SELECT sender, recipient FROM {facebook_status} WHERE sid = %d", $event->target_id));
    $uids = array($status->sender, $status->recipient);
    $result = db_query("SELECT uid FROM {fbss_comments} WHERE uid <> 0 AND sid = %d", $event->target_id);
    while ($account = db_fetch_object($result)) {
      $uids[] = $account->uid;
    }
    return array('user' => $uids);
  }
}

/**
 * Returns the entitity IDs for the Mentioned users group.
 */
function activity_log_eg_mentioned_users($event, $settings) {
  if ($event->target_type == 'facebook_status' && module_exists('facebook_status_tags')) {
    $uids = array();
    $result = db_query("SELECT rid FROM {facebook_status_tags} WHERE type = 'user' AND sid = %d", $event->target_id);
    while ($account = db_fetch_object($result)) {
      $uids[] = $account->rid;
    }
    return array('user' => $uids);
  }
}

/**
 * Returns the entitity IDs for the #Hashtags group.
 */
function activity_log_eg_hashtag_terms($event, $settings) {
  if ($event->target_type == 'facebook_status' && module_exists('facebook_status_tags')) {
    $tids = array();
    $result = db_query("SELECT rid FROM {facebook_status_tags} WHERE type = 'term' AND sid = %d", $event->target_id);
    while ($term = db_fetch_object($result)) {
      $tids[] = $term->rid;
    }
    return array('taxonomy_term' => $tids);
  }
}

/**
 * Returns the entitity IDs for the Node terms group.
 */
function activity_log_eg_node_terms($event, $settings) {
  if ($event->target_type == 'node' && module_exists('taxonomy')) {
    $node = db_fetch_object(db_query("SELECT nid, vid FROM {node} WHERE nid = %d", $event->target_id));
    return array('taxonomy_term' => array_keys(taxonomy_node_get_terms($node)));
  }
}

/**
 * Returns the entitity IDs for the Everyone group.
 */
function activity_log_eg_everyone($event, $settings) {
  return array('user' => array(0));
}

/**
 * Gets an array of UIDs of members of the currently active group.
 */
function _activity_log_get_active_group_members() {
  $result = db_query("SELECT uid FROM {og_uid} WHERE is_active = 1 AND nid = %d", og_get_group_context()->nid);
  $uids = array();
  while ($account = db_fetch_object($result)) {
    $uids[] = $account->uid;
  }
  return $uids;
}

/**
 * Gets an array of UIDs of a given user's relationships.
 *
 * @param $uid
 *   The user ID of the user whose relationships should be retrieved.
 * @param $rtid
 *   If specified, the relationship type ID of the relationships that should be
 *   retrieved. Otherwise, all relationships of all types are retrieved.
 * @return
 *   An array of UIDs of the given user's relationships.
 */
function _activity_log_get_user_relationships($uid, $rtid = 0) {
  if (!$rtid) {
    $result = db_query("
      SELECT ur.requester_id, ur.requestee_id
      FROM user_relationships ur
      INNER JOIN user_relationship_types urt
        ON ur.rtid = urt.rtid
      WHERE (ur.requester_id = %d OR ur.requestee_id = %d) AND (ur.approved = 1 OR urt.is_oneway = 0)
    ", $uid, $uid);
  }
  else {
    $result = db_query("
      SELECT ur.requester_id, ur.requestee_id
      FROM user_relationships ur
      INNER JOIN user_relationship_types urt
        ON ur.rtid = urt.rtid
      WHERE (ur.requester_id = %d OR ur.requestee_id = %d) AND (ur.approved = 1 OR urt.is_oneway = 0) AND ur.rtid = %d
    ", $uid, $uid, $rtid);
  }
  $uids = array();
  while ($rel = db_fetch_object($result)) {
    $uids[] = ($rel->requester_id == $uid) ? $rel->requestee_id : $rel->requester_id;
  }
  return $uids;
}
