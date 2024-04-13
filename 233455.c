static PHP_RINIT_FUNCTION(libxml)
{
	/* report errors via handler rather than stderr */
	xmlSetGenericErrorFunc(NULL, php_libxml_error_handler);
	xmlParserInputBufferCreateFilenameDefault(php_libxml_input_buffer_create_filename);
	xmlOutputBufferCreateFilenameDefault(php_libxml_output_buffer_create_filename);
	return SUCCESS;
}