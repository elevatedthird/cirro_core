<?php

/**
 * Redactor upload page
 */

/**
 * page callback
 * returns json with path
 */
function cirro_js_upload() {

	// Init
	$output = array();

	// Check for files
	if ( isset($_FILES) && !empty($_FILES) ) {

		$rawFile = $_FILES['file'];
		$rawName = $rawFile['name'];

		$_FILES = array(
			'files' => array(
				'name' => array(
					$rawName => $rawName
				),
				'tmp_name' => array(
					$rawName => $rawFile['tmp_name']
				),
				'size' => array(
					$rawName => $rawFile['size']
				),
				'error' => array(
					$rawName => 'UPLOAD_ERR_OK'
				)
			)
		);

		$file = file_save_upload($rawName, array(), 'public://redactor_imgs', FILE_EXISTS_REPLACE);

		variable_set('will_file', $file);

		$file->status = FILE_STATUS_PERMANENT;
		file_save($file);

		$file = file_load($file->fid);
		variable_set('will_file_load', $file);

		$output['filelink'] = file_create_url($file->uri);

	}

	drupal_json_output($output);
}