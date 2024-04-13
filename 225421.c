PHP_NAMED_FUNCTION(php_if_iconv)
{
	char *in_charset, *out_charset;
	zend_string *in_buffer;
	size_t in_charset_len = 0, out_charset_len = 0;
	php_iconv_err_t err;
	zend_string *out_buffer;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ssS",
		&in_charset, &in_charset_len, &out_charset, &out_charset_len, &in_buffer) == FAILURE)
		return;

	if (in_charset_len >= ICONV_CSNMAXLEN || out_charset_len >= ICONV_CSNMAXLEN) {
		php_error_docref(NULL, E_WARNING, "Charset parameter exceeds the maximum allowed length of %d characters", ICONV_CSNMAXLEN);
		RETURN_FALSE;
	}

	err = php_iconv_string(ZSTR_VAL(in_buffer), (size_t)ZSTR_LEN(in_buffer), &out_buffer, out_charset, in_charset);
	_php_iconv_show_error(err, out_charset, in_charset);
	if (err == PHP_ICONV_ERR_SUCCESS && out_buffer != NULL) {
		RETVAL_STR(out_buffer);
	} else {
		if (out_buffer != NULL) {
			zend_string_free(out_buffer);
		}
		RETURN_FALSE;
	}
}