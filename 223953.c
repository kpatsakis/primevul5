ZEND_METHOD(error_exception, __construct)
{
	char  *message = NULL, *filename = NULL;
	long   code = 0, severity = E_ERROR, lineno;
	zval  *object, *previous = NULL;
	int    argc = ZEND_NUM_ARGS(), message_len, filename_len;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC, "|sllslO!", &message, &message_len, &code, &severity, &filename, &filename_len, &lineno, &previous, default_exception_ce) == FAILURE) {
		zend_error(E_ERROR, "Wrong parameters for ErrorException([string $exception [, long $code, [ long $severity, [ string $filename, [ long $lineno  [, Exception $previous = NULL]]]]]])");
	}

	object = getThis();

	if (message) {
		zend_update_property_string(default_exception_ce, object, "message", sizeof("message")-1, message TSRMLS_CC);
	}

	if (code) {
		zend_update_property_long(default_exception_ce, object, "code", sizeof("code")-1, code TSRMLS_CC);
	}

	if (previous) {
		zend_update_property(default_exception_ce, object, "previous", sizeof("previous")-1, previous TSRMLS_CC);
	}

	zend_update_property_long(default_exception_ce, object, "severity", sizeof("severity")-1, severity TSRMLS_CC);

	if (argc >= 4) {
	    zend_update_property_string(default_exception_ce, object, "file", sizeof("file")-1, filename TSRMLS_CC);
    	if (argc < 5) {
    	    lineno = 0; /* invalidate lineno */
    	}
    	zend_update_property_long(default_exception_ce, object, "line", sizeof("line")-1, lineno TSRMLS_CC);
	}
}