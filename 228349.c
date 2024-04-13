PHP_FUNCTION(quoted_printable_encode)
{
	char *str, *new_str;
	int str_len;
	size_t new_str_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) != SUCCESS) {
		return;
	}

	if (!str_len) {
		RETURN_EMPTY_STRING();
	}

	new_str = (char *)php_quot_print_encode((unsigned char *)str, (size_t)str_len, &new_str_len);
	RETURN_STRINGL(new_str, new_str_len, 0);
}