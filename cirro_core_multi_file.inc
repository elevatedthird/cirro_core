<?php

/**
 * @file
 *   All multi_file stuff except for hook_menu and entity_info.
 */

/**
 * Build multi_file weight and upload forms.
 *
 * @param $item
 *   Item array or Add
 * @param array $multi_file_fields
 *   Array of multi_file_fields
 * @param $type
 *   Parent Item type
 * @param $id
 *   Parent Item ID
 *
 * @return
 *   Full markup for multi_file forms or placeholders.
 */
function cirro_core_multi_file_build_forms($item, $multi_file_fields, $type, $id) {
	
	// Init
	$html = '';

	// Check for add item
	if ( $item === 'add' )  {
		
		// Loop through fields
		foreach ( $multi_file_fields as $field_name => $info ) {
		
			// Build placeholder
			$placeholder = "
				<div class=\"multi-form-wrapper\">
					<div class=\"title-wrapper\">
						<h2>{$info['label']}</h2>
					</div>
					<div class=\"note-wrapper\">
						<h4>Finish creating item in order to add images</h4>
					</div>
				</div>
			";	

			// Add placeholder to html
			$html .= $placeholder;
			
		}

	} else {

		// Loop through fields
		foreach ( $multi_file_fields as $field_name => $info ) {
			
			// Query existing file items
			$query_string = "
				SELECT id, title, fid, weight
					FROM cirro_multi_file
					WHERE parent_id = :parent_id
					AND parent_type = :parent_type
					AND parent_field = :parent_field
					ORDER BY weight, created
			";
			
			$args = array(
				':parent_type' => $type,
				':parent_id' => $id,
				':parent_field' => $field_name,
			);
		
			$query = db_query($query_string, $args);
			
			$files = $query->fetchAll();
			
			// Count files
			$files_count = count($files);
			
			// Build form options
			$file_weight_form_options = array(
				'parent_type' => $type,
				'parent_id' => $id,
				'parent_field' => $field_name,
				'files' => $files,
			);
			
			// Build weights form
			$file_weights_form_array = drupal_get_form(
				"cirro_core_{$type}_{$field_name}_multi_file_weights",
				$file_weight_form_options
			);
			
			// Render weight form
			$file_weights_form = drupal_render($file_weights_form_array);
			
			// Check for limit
			if ( isset($info['limit']) && $files_count >= $info['limit'] ) {
				
				// Upload placholder
				$upload_form = "
					<div class=\"alert alert-info\">
						You have reached the upload limit of {$info['limit']} files.<br />
						Delete some files in order to add more.
					</div>
				";
				
			} else {
				
				// Build upload form
				$upload_form_array = drupal_get_form(
					"cirro_core_{$type}_{$field_name}_multi_file_upload",
					$id,
					$type,
					$field_name
				);
				
				// Render upload form
				$upload_form = drupal_render($upload_form_array);
				
				// Build field html
				$field_forms = "
					<div class=\"multi-form-wrapper box-white\">
						<div class=\"title-wrapper\">
							<h2>{$info['label']}</h2>
						</div>
						<div class=\"weight-wrapper\" />
							$file_weights_form
						</div>
						<div class=\"line-seperator\"></div>
						<div class=\"upload-wrapper\">
							<h4>Upload File</h4>
							$upload_form
						</div>
					</div>				
				";
				
				// Add field to html
				$html .= $field_forms;
				
			}

		}
		
	}
	
	return $html;

}

/**
 * Multi File weight form.
 *
 * @parm $id
 *   Parent ID
 * @param $type
 *   Entity type
 * @param $field
 *   Entity field name
 * @param $files
 *   Array of file items
 */
function cirro_core_multi_file_weight_form($form, &$form_state, $options) {

	// Init
	$form = array();
	$id = $options['parent_id'];
	$type = $options['parent_type'];
	$field = $options['parent_field'];
	$files = $options['files'];

	// Build weight options
	$weight_options = array();

	for ( $i = -40; $i <= 40; $i++ ) {

		$weight_options[$i] = $i;

	}

	// Add parent info to form
	$form['parent_id'] = array(
		'#type' => 'hidden',
		'#value' => $id,
	);

	$form['parent_type'] = array(
		'#type' => 'hidden',
		'#value' => $type,
	);

	$form['parent_field'] = array(
		'#type' => 'hidden',
		'#value' => $field,
	);

	// Setup files array
	$form['files'] = array();
	$form['files']['#tree'] = TRUE;

	// Loop through files
	foreach ( $files as $file ) {

		// Check for fid of 0
		if ( $file->fid == 0 ) {

			// Delete multi_file
			entity_get_controller('multi_file')->delete($file->id);
			
			// Continue on in loop
			continue;
			
		}

		// Load file object
		$file_obj = file_load($file->fid);

		// Load file info
		$file_name = $file_obj->filename;
		$file_uri = $file_obj->uri;
		$file_path = file_create_url($file_uri);

		// Init fields array
		$fields = array();

		$fields['id'] = array(
			'#type' => 'hidden',
			'#value' => $file->id,
		);

		$fields['file_name'] = array(
			'#markup' => l(
				$file_name,
				$file_path,
				array(
					'html' => TRUE,
					'attributes' => array('target' => '_blank'),
				)
			),
		);

		$fields['weight'] = array(
			'#type' => 'select',
			'#title' => 'Weight',
			'#default_value' => $file->weight,
			'#options' => $weight_options,
		);

		// Add fields
		$fields['title'] = array(
			'#type' => 'textfield',
			'#attributes' => array(
				'placeholder' => 'Title',
				'title' => 'Title',
			),
			'#default_value' => $file->title,
		);

		$fields['delete_link'] = array(
			'#markup' => l(
				'<i class="icon-remove"></i> ',
				"admin/dash/multi_file/{$file->id}/delete",
				array(
					'html' => TRUE,
					'attributes' => array(
						'class' => array('btn', 'btn-small'),
						'title' => 'Delete File',
					),
					'query' => array(
						'destination' => "admin/dash/$type/$id/edit"
					),
				)
			),
		);

		// Add onto files index
		$form['files'][$file->id] = $fields;

	}

	if ( !empty($files) ) {
	
		// Add submit
		$form['submit'] = array(
			'#type' => 'submit',
			'#value' => 'Update Files',
		);
		
	}
	
	return $form;
	
}

/**
 * Multi File weights submit
 */
function cirro_core_multi_file_weight_form_submit($form, &$form_state) {

	// Init
	$values = $form_state['values'];

	$objects = array();

	// Loop through files
	foreach ( $values['files'] as $id => $data ) {

		// Cast fields object
		$fields_obj = (object) $data;

		// Update file
		entity_get_controller('multi_file')->update($fields_obj);

	}

	// Display success message
	drupal_set_message('Files Updated');

}

/**
 * Multi File upload form
 *
 * @param $parent_id
 *   Parent entity id
 * @param $parent_type
 *   Parent entity type
 * @param $parent_field
 *   Parent entity field name
 */
function cirro_core_multi_file_upload_form($form, &$form_state, $id, $type, $field) {

	// Init
	$form = array();

	// Parent id
	$form['parent_id'] = array(
		'#type' => 'hidden',
		'#value' => $id,
	);

	// Type field
	$form['parent_type'] = array(
		'#type' => 'hidden',
		'#value' => $type,
	);

	// Field name
	$form['parent_field'] = array(
		'#type' => 'hidden',
		'#value' => $field,
	);
	
	// Weight
	$form['weight'] = array(
		'#type' => 'hidden',
		'#value' => 40,
	);

	// Title Field
	$form['title'] = array(
		'#type' => 'textfield',
		'#title' => 'Title',
		'#required' => TRUE,
	);

	// File field
	$form['fid'] = array(
		'#type' => 'managed_file',
		'#upload_location' => "public://multi_file/$type/$field",
		'#upload_validators' => array(
			'file_validate_size' => array(1048576),
		),
	);

	// Submit
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => 'Save File',
	);

	return $form;
}

/**
 * Multi File Upload submit.
 */
function cirro_core_multi_file_upload_form_submit($form, &$form_state) {

	// Init
	$fields = $form_state['values'];


	// Cast fields obj
	$fields_obj = (object) $fields;

	// Attempt item create
	if ( ( $item = entity_get_controller('multi_file')->create($fields_obj) ) !== FALSE ) {

		drupal_set_message('File Created');

	} else {

		drupal_set_message('File creation failure', 'error');

	}

}


/**
 * Theme multi file weights forms.
 *
 * @param $variables
 *   Themeing variables array
 */
function theme_cirro_core_multi_file_weight_form($variables) {
	
	// Init
	$output = '';
	$rows = array();
	
	// Load form array
	$form = $variables['form'];
	
	// Activate tabledrag shiz
	drupal_add_tabledrag('draggable-table', 'order', 'sibling', 'weight-group');
	
	// Loop through form Elements
	foreach ( element_children($form['files']) as $key ) {
		
		// Init
		$element = array();
		$data = '';
		$row = array();
		
		// Skip none numeric keys
		if ( !is_numeric($key) ) continue;
		
		// Load element
		$element = &$form['files'][$key];
		
		// Add class to weight field
		$element['weight']['#attributes']['class'] = array('weight-group');
		
		// Put data fields together
		$data .= drupal_render($element['id']);
		$data .= drupal_render($element['title']);
		
		// Build row
		$row = array();
		$row[] = drupal_render($element['file_name']);
		$row[] = $data;
		$row[] = drupal_render($element['weight']);
		$row[] = drupal_render($element['delete_link']);
		
		// Add to rows
		$rows[] = array( 'data' => $row, 'class' => array('draggable'));

	}
	
	// Define headers
	$headers = array(
		'File',
		'Data',
		'Weight',
		'',
	);

	// Define table vars
	$table_vars = array(
		'header' => $headers,
		'rows' => $rows,
		'attributes' => array(
			'id' => 'draggable-table',
			'class' => array(
				'table',
				'table-bordered',
				'table-striped'
			),
		),
		'caption' => 'Drag and Drop to change order',
		'colgroups' => array(),
		'sticky' => FALSE,
		'empty' => 'No Files found',
	);
	
	// Add table to output
	$output = theme_table($table_vars);
	
	// Render remaining form elements
	$output .= drupal_render_children($form);	
	
	return $output;

}

/**
 * Multi-file load helper function.
 *
 * Query all files for a parent type, id, and field.
 *
 * @return
 *   Array of multi_file item objects with pre-built URL.
 */
function cirro_core_multi_file_load_helper($type, $id, $field){
 
 // Init stuff here
 $files = array();
 
	
	// Query items
	$file_query_string = "
	SELECT i.title, f.uri, f.fid 
  	FROM cirro_multi_ AS i
  	JOIN file_managed AS f
  		ON i.fid = f.fid
  	WHERE i.parent_id = :id
  	AND i.parent_type = :type
  	AND i.parent_field = :field
  	ORDER BY i.weight
	";
	
	$placeholders = array(
		':id' => $id,
		':type' => $type,
		':field' => $field,
	);
	
	$file_query = db_query($file_query_string, $placeholders);
	$file_results = $file_query->fetchAll();
 
 // If images return url, title, alt
 foreach ( $file_results as $file_result ) {
	 
	 $file_result->url = file_create_url($file_result->uri);
	 
	 $files[] = $file_result;

 }

 return $files;	 
}