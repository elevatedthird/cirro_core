<?php
/**
 * @file
 *   All admin forms.
 */

/**
 * Add/Edit form page callback.
 */
function cirro_core_entity_edit_page($type, $item, $action = NULL) {

	// Init
	$html = '';
	$info = entity_get_info($type);
	$schema = drupal_get_schema($info['base table']);
	$id_field = $info['entity keys']['id'];
	$children = '';
	$imgs = '';

	// Load primary form
	$form_array = drupal_get_form('cirro_core_entity_edit_form', $info, $schema, $type, $item, $action);
	$form = drupal_render($form_array);

	// Add form to html
	$form_wrapped = "
		<div class=\"edit-form-wrapper box-white\">
			$form
		</div>
	";

	// Check for Children
	if ( isset($info['children']) ) {
		
		// Load children
		$children_types = $info['children'];
		
		// Check for Add page
		if ( $item === 'add' ) {
		
			// Loop through children
			foreach ( $children_types as $child_type => $child_label ) {
				
				// Build child form placeholder
				$child_form = "
					<div class=\"children-form-wrapper\">
						<div class=\"title-wrapper\">
							<h2>$child_label</h2>
						</div>
						<div class=\"note-wrapper\">
							<h4>Finish creating item in order to add {$child_label}s</h4>
						</div>
					</div>
				";
				
				// Add to children markup
				$children .= $child_form;
				
				
			}
		
		
		} else {
			
			// Edit page
			
			
			
		}
		
	}

	// Check for multi_img in schema
	if ( isset($info['multi_img']) ) {

		// Load multi imgs
		$multi_imgs = $info['multi_img'];

		// Check for add
		if ( $item === 'add' ) {

			// Loop through multi-imgs
			foreach ( $multi_imgs as $name => $info ) {

				// Build multi img add placeholder
				$img_form = "
					<div class=\"multi-img-form-wrapper\">
						<div class=\"title-wrapper\">
							<h2>{$info['label']}</h2>
						</div>
						<div class=\"note-wrapper\">
							<h4>Finish creating item in order to add images</h4>
						</div>
					</div>
				";

				// Add to imgs markup
				$imgs .= $img_form;

			}

		} else {

			// Load ID
			$id = $item->$id_field;	

			// Loop through multi-imgs
			foreach ( $multi_imgs as $field => $info ) {

				// Query existing imgs
				$query_string = "
					SELECT id, fid, weight, img_alt, img_title
						FROM cirro_multi_img
						WHERE parent_id = :id
						AND parent_type = :type
						AND parent_field = :field
						ORDER BY weight, created
				";
			
				$placeholders = array(
					':id' => $id,
					':type' => $type,
					':field' => $field
				);
			
				$query = db_query($query_string, $placeholders);
			
				$imgs = $query->fetchAll();
				
				// Count imgs
				$img_count = count($imgs);

				// Build img weight form options
				$img_weight_options = array(
					'id' => $id,
					'type' => $type,
					'field' => $field,
					'imgs' => $imgs,
				);

				// Get img weight form
				$weight_form_array = drupal_get_form(
					"cirro_core_{$type}_{$field}_multi_img_weights", $img_weight_options);

				if ( isset($info['limit']) && $img_count >= $info['limit'] ) {
					
					// Replace upload field with limit warning
					$upload_form = "
						<div class=\"alert alert-info\">
							You have reached the upload limit of {$info['limit']} images.<br />
							Delete some images in order to add more.
						</div>
					";
					
				} else {

					// Load forms
					$upload_form_array = drupal_get_form(
						'cirro_core_multi_img_upload_form',
						$id,
						$type,
						$field
					);
					
					// Render upload form
					$upload_form = drupal_render($upload_form_array);
					
				}

				// Render forms
				$weight_form = drupal_render($weight_form_array);

				// Build multi img form elements
				$img_form = "
					<div class=\"multi-img-form-wrapper box-white\">
						<div class=\"title-wrapper\">
							<h2>{$info['label']}</h2>
						</div>
						<div class=\"weight-wrapper\" />
							$weight_form
						</div>
						<div class=\"line-seperator\"></div>
						<div class=\"upload-wrapper\">
							<h4>Upload Image</h4>
							$upload_form
						</div>
					</div>
				";

				// Add to imgs markup
				$imgs .= $img_form;

			}

		}

	}

	// Build html
	$html = "
		<div class=\"row-fluid\">
			<div class=\"span6\">
				$form_wrapped
			</div>
			<div class=\"span6\">
				$children
				$imgs
			</div>
		</div>
	";

	return $html;

}

/**
 * CRUD create/update form.
 *
 * @param $type
 *   Entity Type
 *
 * @param $id
 *   ID of item being updated OR the string 'create'
 *
 * @return
 *   Form array
 */
function cirro_core_entity_edit_form($form, &$form_state, $entity_info, $schema, $type, $item, $action = NULL) {

	// Init
	$form = array();
	$entity_label = $entity_info['label'];
	$base_table = $entity_info['base table'];
	$id_field = $entity_info['entity keys']['id'];
	$fields = $schema['fields'];

	// Check for id
	if ( $item === 'add' ) {

		// Create empty object
		$item = new stdClass;

		// Unset id field
		unset($fields[$id_field]);

		// Set action
		$action = 'add';

	}

	// Add type as hidden element
	$form['type'] = array(
		'#type' => 'hidden',
		'#value' => $type,
	);

	// Add type label as hidden element
	$form['type_label'] = array(
		'#type' => 'hidden',
		'#value' => $entity_label,
	);

	// Add action as hidden element
	$form['action'] = array(
		'#type' => 'hidden',
		'#value' => $action,
	);

	// Add id field as hidden element
	$form['id_field'] = array(
		'#type' => 'hidden',
		'#value' => $id_field,
	);

	// If the item has a value for the id_field
	if ( isset($item->id) ) {

		// Add id value as hidden element
		$form[$id_field] = array(
			'#type' => 'hidden',
			'#value' => $item->id,
		);

	}

	// Uppercase action for display
	$action_display = ucfirst($action);

	// Loop through schema fields
	foreach ( $fields as $field => $info ) {

		// Look for form array within field info
		if ( isset($info['form']) ) {

			// Add to form
			$form[$field] = $info['form'];

			// Check for default value
			if ( isset($item->$field) ) {

				// Add default value to field
				$form[$field]['#default_value'] = $item->$field;

			}

		}

	}

	// Submit
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => "Save Item",
	);	

	return $form;

}

/**
 * Submit entity create/update
 *
 */
function cirro_core_entity_edit_form_submit($form, &$form_state) {

	// Init
	$values = $form_state['values'];
	$type = $values['type'];
	$action = $values['action'];
	$id_field = $values['id_field'];
	$file_fields = array();

	// Translate action to method
	if ( $action === 'add' ) $action = 'create';
	if ( $action === 'edit' ) $action = 'update';

	// Cast fields object
	$fields_obj = (object) $values;

	if ( ( $new_obj = entity_get_controller($type)->$action($fields_obj) ) !== FALSE ) {

		// Load id
		$id = $new_obj->$id_field;

		// Set success message
		drupal_set_message("Item $action complete.");

		// Redirect to display
		$form_state['redirect'] = "admin/dash/$type/$id/edit";

	} else {
		drupal_set_message("Controller fail.", "error");
	} 

}

/**
 * Delete confirmation form
 *
 * @param $type
 *   Entity machine name
 *
 * @param $item
 *   Item loaded object
 *
 */
function cirro_core_entity_delete_form($form, &$form_state, $type, $item) {

	// Init
	$form = array();
	$entity_info = entity_get_info($type);
	$entity_label = $entity_info['label'];
	$base_table = $entity_info['base table'];
	$schema = drupal_get_schema($base_table);
	$primary_key = reset($schema['primary key']);
	$id = $item->$primary_key;
	$name = "<em>$entity_label Item</em>";

	// Check for item name
	if ( isset($item->name) ) {

		// Load item name
		$name = "<em>{$item->name}</em>";

	}

	// Add type to form
	$form['type'] = array(
		'#type' => 'hidden',
		'#value' => $type,
	);

	// Add id to form
	$form['id'] = array(
		'#type' => 'hidden',
		'#value' => $id,
	);

	// Build confirm args
	$question = "Confirm Delete: $name";
	$path = "admin/dash/$type/$id/edit";
	$description = "
		Are you sure you want to delete <strong>$name</strong>?<br />
		This cannot be undone!";

	// Build confirm form
	$form = confirm_form($form, $question, $path, $description);

	return $form;

}

/**
 * Delete submit
 */
function cirro_core_entity_delete_form_submit($form, &$form_state) {

	// Init
	$values = $form_state['values'];
	$type = $values['type'];
	$id = $values['id'];
	$type_info = entity_get_info($type);

	// Attempt delete
	if ( entity_get_controller($type)->delete($id) !== FALSE ) {

		// Display message
		drupal_set_message('Successfully deleted.');

		// Redirect to dashboard
		$form_state['redirect'] = 'admin/dash';

	}

}

/**
 * Weights reorder form.
 */
function cirro_core_weights_form($form, &$form_state, $type) {
	
	// Init
	$info = entity_get_info($type);
	$label = $info['label'];
	$id_field = $info['entity keys']['id'];
	$form = array();
	$form['items'] = array();
	$form['items']['#tree'] = TRUE;
	
	// Build weight options
	$weight_options = array();

	for ( $i = -40; $i <= 40; $i++ ) {

		$weight_options[$i] = $i;

	}

	// Query items
	$query_string = "
		SELECT id, name, weight
			FROM cirro_$type
			ORDER BY weight, name
	";
	
	$query = db_query($query_string);
	
	$items = $query->fetchAll();
	
	// Add type to form
	$form['type'] = array(
		'#type' => 'hidden',
		'#value' => $type,
	);
	
	// Add id_field t- form
	$form['id_field'] = array(
		'#type' => 'hidden',
		'#value' => $id_field,
	);
	
	// Loop through items
	foreach ( $items as $item ) {
		
		// Build fields
		$fields = array();
		
		// ID
		$fields[$id_field] = array(
			'#type' => 'hidden',
			'#value' => $item->$id_field,
		);

		// NAME
		$fields['name'] = array(
			'#markup' => "<h3>{$item->name}</h3>",
		);

		// WEIGHT
		$fields['weight'] = array(
			'#type' => 'select',
			'#title' => 'Weight',
			'#options' => $weight_options,
			'#default_value' => $item->weight,
		);

		// Add fields to items
		$form['items'][$item->$id_field] = $fields;

	}

	// SUBMIT
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => 'Save',
	);
	
	return $form;
	
}

/**
 * Item weights form submit.
 */
function cirro_core_weights_form_submit($form, &$form_state) {
	
	// Init
	$values = $form_state['values'];
	$type = $values['type'];

	$objects = array();

	// Loop through imgs
	foreach ( $values['items'] as $id => $data ) {

		// Cast fields object
		$fields_obj = (object) $data;

		// Update img
		entity_get_controller($type)->update($fields_obj);

	}

	// Display success message
	drupal_set_message('Item order updated');
	
}

/**
 * Multi Image File upload form
 *
 * @param $type
 *   Entity type
 * @param $field
 *   Entity field name
 */
function cirro_core_multi_img_upload_form($form, &$form_state, $id, $type, $field) {

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

	// File field
	$form['fid'] = array(
		'#type' => 'managed_file',
		'#upload_location' => "public://multi_imgs/$type/$field",
		'#upload_validators' => array(
			'file_validate_extensions' => array('jpg jpeg gif png'),
			'file_validate_size' => array(1048576),
		),
	);

	// Submit
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => 'Save Img',
	);

	return $form;
}

/**
 * Multi Image File Upload submit.
 */
function cirro_core_multi_img_upload_form_submit($form, &$form_state) {

	// Init
	$fields = $form_state['values'];


	// Cast fields obj
	$fields_obj = (object) $fields;

	// Attempt item create
	if ( ( $item = entity_get_controller('multi_img')->create($fields_obj) ) !== FALSE ) {

		drupal_set_message('Image created');

	} else {

		drupal_set_message('Image creation failure', 'error');

	}

}

/**
 * Multi Img weight form.
 *
 * @parm $id
 *   Parent ID
 * @param $type
 *   Entity type
 * @param $field
 *   Entity field name
 * @param $imgs
 *   Array of img items
 */
function cirro_core_multi_img_weight_form($form, &$form_state, $options) {

	// Init
	$form = array();
	$id = $options['id'];
	$type = $options['type'];
	$field = $options['field'];
	$imgs = $options['imgs'];

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

	// Setup imgs array
	$form['imgs'] = array();
	$form['imgs']['#tree'] = TRUE;

	// Loop through images
	foreach ( $imgs as $img ) {

		// Check for fid of 0
		if ( $img->fid == 0 ) {

			// Delete multi_img
			entity_get_controller('multi_img')->delete($img->id);
			
			// Continue on in loop
			continue;
			
		}

		// Load file object
		$file = file_load($img->fid);

		// Load file info
		$file_name = $file->filename;
		$file_uri = $file->uri;
		$file_path = file_create_url($file_uri);

	  $img_styled = '';

	  // Load image vars
	  $img_vars = array(
	  	'style_name' => 'thumbnail',
	  	'path' => $file_uri,
	  	'alt' => $file_name,
	  	'title' => $file_name,
	  	'width' => NULL,
	  	'height' => NULL,
	  );

	  // Theme/style image
	  $img_styled =  theme_image_style($img_vars);

		// Init fields array
		$fields = array();

		$fields['id'] = array(
			'#type' => 'hidden',
			'#value' => $img->id,
		);

		$fields['thumbnail'] = array(
			'#markup' => l(
				$img_styled,
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
			'#default_value' => $img->weight,
			'#options' => $weight_options,
		);

		// Add fields
		$fields['img_title'] = array(
			'#type' => 'textfield',
			'#attributes' => array(
				'placeholder' => 'Title Text',
				'title' => 'Title Text',
			),
			'#default_value' => $img->img_title,
		);

		$fields['img_alt'] = array(
			'#type' => 'textfield',
			'#attributes' => array(
				'placeholder' => 'Alt Text',
				'title' => 'Alt Text',
			),
			'#default_value' => $img->img_alt,
		);

		$fields['delete_link'] = array(
			'#markup' => l(
				'<i class="icon-remove"></i> ',
				"admin/dash/multi_img/{$img->id}/delete",
				array(
					'html' => TRUE,
					'attributes' => array(
						'class' => array('btn', 'btn-small'),
						'title' => 'Remove Image',
					),
					'query' => array(
						'destination' => "admin/dash/$type/$id/edit"
					),
				)
			),
		);

		// Add onto imgs index
		$form['imgs'][$img->id] = $fields;

	}

	if ( !empty($img) ) {
	
		// Add submit
		$form['submit'] = array(
			'#type' => 'submit',
			'#value' => 'Update Images',
		);
		
	}
	
	return $form;
	
}

/**
 * Multi Img weights submit
 */
function cirro_core_multi_img_weight_form_submit($form, &$form_state) {

	// Init
	$values = $form_state['values'];

	$objects = array();

	// Loop through imgs
	foreach ( $values['imgs'] as $id => $data ) {

		// Cast fields object
		$fields_obj = (object) $data;

		// Update img
		entity_get_controller('multi_img')->update($fields_obj);

	}

	// Display success message
	drupal_set_message('Images updated');

}