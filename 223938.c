ZEND_METHOD(exception, __construct)
{
	char  *message = NULL;
	long   code = 0;
	zval  *object, *previous = NULL;
	int    argc = ZEND_NUM_ARGS(), message_len;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC, "|slO!", &message, &message_len, &code, &previous, default_exception_ce) == FAILURE) {
		zend_error(E_ERROR, "Wrong parameters for Exception([string $exception [, long $code [, Exception $previous = NULL]]])");
	}

	object = getThis();

	if (message) {
		zend_update_property_stringl(default_exception_ce, object, "message", sizeof("message")-1, message, message_len TSRMLS_CC);
	}

	if (code) {
		zend_update_property_long(default_exception_ce, object, "code", sizeof("code")-1, code TSRMLS_CC);
	}

	if (previous) {
		zend_update_property(default_exception_ce, object, "previous", sizeof("previous")-1, previous TSRMLS_CC);
	}
}