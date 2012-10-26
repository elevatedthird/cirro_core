/**
 * The Clim Admin Theme JS plugin instantiations
 *
 * This script is called in the footer before all other instantiations
 * within the climAdmin theme.
 */
;(function( Cirro, $, window, document, undefined ){

	"use strict";


	/**
	 * Cirro WYSIWYG Validation
	 */

	// If the Redactor jQuery plugin is not defined
	if ( typeof $.fn.redactor !== "function" ) {

		// Call error and stop the function
		Cirro.error( "The Redactor Plugin was not added to the page." );
		return;

	}

	// If the Cirro WYSIWYG object has been overridden and is now undefined
	if ( 
		typeof Cirro.WYSIWYG !== "object" ||
		typeof Cirro.WYSIWYG.fieldSelector !== "string" ||
		typeof Cirro.WYSIWYG.buttons !== "object"
	) {

		// Call error and stop the function
		Cirro.error( "The WYSIWYG options are missing." );
		return;
		
	}

	/**
	 * Document Ready and Plugin Instantiations
	 */

	// Cache references to the jQuery window and document
	var $document = $( document );

	// When the document is ready
	$document.ready(function() {

		// Init
		var $textareas = $( Cirro.WYSIWYG.fieldSelector );
		var buttons = Cirro.WYSIWYG.buttons;

console.log( $textareas );

		// Instantiate redactor
		$textareas.redactor({
			buttons: buttons,
		});

	});	

}( window.Cirro = window.Cirro || {}, jQuery, window, document ));