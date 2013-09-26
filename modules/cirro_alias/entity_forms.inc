<?php

/**
 * Forms and such for entity alias stuff.
 */

/**
 * Build alias section.
 */
function cirro_alias_entity_build_forms($item, $url_info) {
	// Check for perms
	if ( !user_access('cirro manage alias') ) return '';

	// Init
	$html = '';

	// Check for Add
	if ( $item === 'add' ) {
		$html = '
			<div class=\"alias-form-wrapper\">
				<div class=\"title-wrapper\">
					<h2>URL Aliases</h2>
				</div>
				<div class=\"note-wrapper\">
					<h4>Finish creating item in order manage aliases</h4>
				</div>
			</div>
		';

		return $html;
	}

	// Secondary Init
	$link = '';
	$aliases = '';
	$id = $item->id;

	// Build view link
	$root = str_replace('%', $item->id, $url_info['root']);
	$url_info['root'] = $root;
	$options = array(
		'attributes' => array(
			'class' => array('btn', 'btn-primary'),
			'target' => '_blank'
		)
	);
	$link = "<a class=\"btn btn-primary\"href=\"/$root\">View Item</a>";

	// Check for alias
	if ( isset($url_info['alias']) && $url_info['alias'] ) {

		$rows = '';

		// Alias Table
		$query_string = '
			SELECT *
				FROM url_alias
				WHERE source = :root
				ORDER BY alias
		';
		$query = db_query($query_string, array(':root' => $root));
		$paths = $query->fetchAll();

		foreach ( $paths as $path ) {

			$pid = $path->pid;
			$alias = $path->alias;

			$options = array(
				'html' => TRUE,
				'query' => array(
					'destination' => current_path(),
				),
				'attributes' => array(
					'class' => array('btn btn-small'),
					'title' => 'Delete Alias'
				),
			);
			$icon = '<i class="icon-remove"></i> ';
			$delete = l($icon, "admin/dash/alias/$pid/delete", $options);

			$options = array(
				'html' => TRUE,
				'attributes' => array(
					'class' => array('btn btn-small'),
					'target' => '_blank',
				),
			);
			$test = l('Test', $alias, $options);

			$rows .= "
				<tr><td>$alias</td><td>$test $delete</td></tr>
			";
		}

		$table = "
			<table class=\"table table-bordered table-striped\">
				<thead>
					<tr><th>Alias</th><th>Actions</th></tr>
				</thead>
				<tbody>
					$rows
				</tbody>
			</table>
		";

		$aliases .= $table;

		// Add form
		$form = drupal_get_form('cirro_alias_entity_form', $url_info, $id);
		$add_form = drupal_render($form);

		$aliases .= $add_form;

	}

	$html = "
		<div class=\"alias-form-wrapper\">
			<div class=\"title-wrapper\">
				<h2>URL Aliases</h2>
			</div>
			$link
			$aliases
		</div>
	";

	return $html;
}

/**
 * Build alias add form.
 */
function cirro_alias_entity_form($form, &$form_state, $url_info, $id) {
	// Init
	$form = array();
	$root = $url_info['root'];

	$form['id'] = array(
		'#type' => 'hidden',
		'#value' => $id
	);

	$form['root'] = array(
		'#type' => 'hidden',
		'#value' => $root
	);

	$form['alias'] = array(
		'#type' => 'textfield',
		'#title' => 'Alias URL',
		'#required' => TRUE,
	);

	// Check for prefix
	if ( isset($url_info['prefix']) ) {
		$prefix = $url_info['prefix'];
		// Default value
		$form['alias']['#default_value'] = $prefix;

		// Description
		$description = "It is recommended to use a prefix of $prefix";
		$form['alias']['#description'] = $description;
	}

	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => 'Save',
		// '#disabled' => TRUE,
	);

	return $form;
}

/**
 * Validate alias form.
 */
function cirro_alias_entity_form_validate($form, &$form_state) {

	$alias = cirro_alias_cleanup($form_state['values']['alias']);

	// Check if alias alrady exists
	$query_string = "
		SELECT *
			FROM url_alias
			WHERE alias = :alias
	";
	$query = db_query($query_string, array(':alias' => $alias));

	if ( ($path = $query->fetchObject()) !== FALSE ) {
		form_set_error('alias', "'$alias' is already an alias for $path->source");
	}

}

/**
 * Submit alias form
 */
function cirro_alias_entity_form_submit($form, &$form_state) {
	$values = $form_state['values'];
	$root = $values['root'];
	$alias = cirro_alias_cleanup($values['alias']);

	$path = array(
		'source' => $root,
		'alias' => $alias,
	);

	path_save($path);

	drupal_set_message('Alias Saved');
}

/**
 * Alias delete form
 */
function cirro_alias_delete_form($form, &$form_state, $pid) {

	// Init
	$form = array();
	$title = 'Delete Alias';
	$yes = 'Yes, I am sure. Delete this alias forever!';

	// Get breadcrumb
	// $breadcrumb = cirro_core_build_breadcrumb($item);
	
	// Replace item text with link
	// array_pop($breadcrumb);
	// $url = "admin/dash/{$item->type}/$id/edit";
	// $breadcrumb[] = l($title, $url);
	
	// // Add delete text
	// $breadcrumb[] = 'DELETE';
	
	// drupal_set_breadcrumb($breadcrumb);

	// Add pid to form
	$form['pid'] = array(
		'#type' => 'hidden',
		'#value' => $pid,
	);

	// Build confirm args
	$question = "Confirm Alias Delete";
	$path = "admin/dash/alias";
	$description = "
		
		Are you sure you want to delete this URL alias?<br />
		<div class=\"alert\">This cannot be undone!</div>
	";

	// Build confirm form
	$form = confirm_form($form, $question, $path, $description, $yes);

	return $form;

}

/**
 * Alias delete form
 */
function cirro_alias_delete_form_submit($form, &$form_state) {

	// Init
	$pid = $form_state['values']['pid'];

	path_delete($pid);

	drupal_set_message('Alias deleted');

}