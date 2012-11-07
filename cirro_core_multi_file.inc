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


























