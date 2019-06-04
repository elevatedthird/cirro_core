<?php

/**
 * Global alias management forms.
 */

/**
 * Management Page Callback.
 */
function cirro_alias_page() {
	// Build breadcrumb
	$breadcrumb = array();
	$breadcrumb[] = l('Dashboard', 'admin/dash');
	$breadcrumb[] = 'Aliases';

	drupal_set_breadcrumb($breadcrumb);

	// Init
	$html = '';
	$rows = '';

	// Build Add Form
	$form = drupal_get_form('cirro_alias_form');
	$form = drupal_render($form);

	$html .= $form;

	// Get aliases
	$query_string = '
		SELECT *
		FROM {url_alias}
		ORDER BY source
	';
	$query = db_query($query_string);
	$paths = $query->fetchAll();

	// Build table rows
	foreach ( $paths as $path ) {

		$source = "<a href=\"/$path->source\" target=\"_blank\">$path->source</a>";
		$alias = "<a href=\"/$path->alias\" target=\"_blank\">$path->alias</a>";

		$options = array(
			'html' => TRUE,
			'attributes' => array(
				'class' => array('btn', 'btn-small')
			),
			'query' => array(
				'destination' => 'admin/dash/alias'
			)
		);
		$icon = '<i class="icon-remove"></i> ';
		$delete = l($icon, "admin/dash/alias/$path->pid/delete", $options);

		$rows .= "
			<tr>
				<td>$source</td>
				<td>$alias</td>
				<td>$delete</td>
			</tr>
		";
	}

	$table = "
		<table class=\"table table-bordered table-striped\">
			<thead>
				<tr>
					<th>Source</th>
					<th>Alias</th>
					<th></th>
			</thead>
			<tbody>
				$rows
			</tbody>
		</table>
	";

	$html .= $table;

	return $html;
}

/**
 * Add/edit alias form
 */
function cirro_alias_form($form, &$form_state) {
	// Init
	$form = array();

	$form['source'] = array(
		'#type' => 'textfield',
		'#title' => 'Source URL',
		'#required' => TRUE
	);

	$form['alias'] = array(
		'#type' => 'textfield',
		'#title' => 'Alias URL',
		'#required' => TRUE
	);

	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => 'Save'
	);

	return $form;
}

/**
 * Validate add/edit form.
 */
function cirro_alias_form_validate(&$form, &$form_state) {
	$alias = cirro_alias_cleanup($form_state['values']['alias']);

	// Check if alias alrady exists
	$query_string = '
		SELECT *
		FROM {url_alias}
		WHERE alias = :alias
	';
	$query = db_query($query_string, array(':alias' => $alias));

	if ( ($path = $query->fetchObject()) !== FALSE ) {
		form_set_error('alias', "'$alias' is already an alias for $path->source");
	}
}

/**
 * Add/edit form submit
 */
function cirro_alias_form_submit(&$form, &$form_state) {
	$values = $form_state['values'];
	$alias = cirro_alias_cleanup($values['alias']);

	$path = array(
		'source' => $values['source'],
		'alias' => $alias,
	);

	path_save($path);

	drupal_set_message('Alias saved');

}
