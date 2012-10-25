/**
 * The Clim Admin Theme JS plugin instantiations
 *
 * This script is called in the footer before all other instantiations
 * within the climAdmin theme.
 */
;(function( Cirro, $, window, document, undefined ){

	"use strict";

	/**
	 * Cirro WYSIWYG instantiation
	 */

	// If the Cirro WYSIWYG object has not been defined or overwritten
	if ( typeof Cirro.WYSIWYG !== "object"	) {

		// ... this is where I left off
		
	}

	// Cache references to the jQuery window and document
	var $document = $( document );

	// When the document is ready
	$document.ready(function() {

		// Init
		var $textareas = $( Cirro.WYSIWYG.fieldSelector );
		var buttons = Cirro.WYSIWYG.buttons;

		// Instantiate redactor
		$textareas.redactor({
			buttons: buttons,
		});

	});	

}( window.Cirro = window.Cirro || {}, jQuery, window, document ));