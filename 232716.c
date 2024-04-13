PHP_FUNCTION( msgfmt_parse )
{
	char *source;
	size_t source_len;
	MSG_FORMAT_METHOD_INIT_VARS;


	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "Os",
		&object, MessageFormatter_ce_ptr,  &source, &source_len ) == FAILURE )
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"msgfmt_parse: unable to parse input params", 0 );

		RETURN_FALSE;
	}

	/* Fetch the object. */
	MSG_FORMAT_METHOD_FETCH_OBJECT;

	msgfmt_do_parse(mfo, source, source_len, return_value);
}