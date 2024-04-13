PHP_FUNCTION(iconv_set_encoding)
{
	char *type;
	zend_string *charset;
	size_t type_len, retval;
	zend_string *name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sS", &type, &type_len, &charset) == FAILURE)
		return;

	if (ZSTR_LEN(charset) >= ICONV_CSNMAXLEN) {
		php_error_docref(NULL, E_WARNING, "Charset parameter exceeds the maximum allowed length of %d characters", ICONV_CSNMAXLEN);
		RETURN_FALSE;
	}

	if(!strcasecmp("input_encoding", type)) {
		name = zend_string_init("iconv.input_encoding", sizeof("iconv.input_encoding") - 1, 0);
	} else if(!strcasecmp("output_encoding", type)) {
		name = zend_string_init("iconv.output_encoding", sizeof("iconv.output_encoding") - 1, 0);
	} else if(!strcasecmp("internal_encoding", type)) {
		name = zend_string_init("iconv.internal_encoding", sizeof("iconv.internal_encoding") - 1, 0);
	} else {
		RETURN_FALSE;
	}

	retval = zend_alter_ini_entry(name, charset, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
	zend_string_release(name);

	if (retval == SUCCESS) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}