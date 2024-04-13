void zend_exception_set_previous(zval *exception, zval *add_previous TSRMLS_DC)
{
	zval *previous;

	if (exception == add_previous || !add_previous || !exception) {
		return;
	}
	if (Z_TYPE_P(add_previous) != IS_OBJECT && !instanceof_function(Z_OBJCE_P(add_previous), default_exception_ce TSRMLS_CC)) {
		zend_error(E_ERROR, "Cannot set non exception as previous exception");
		return;
	}
	while (exception && exception != add_previous && Z_OBJ_HANDLE_P(exception) != Z_OBJ_HANDLE_P(add_previous)) {
		previous = zend_read_property(default_exception_ce, exception, "previous", sizeof("previous")-1, 1 TSRMLS_CC);
		if (Z_TYPE_P(previous) == IS_NULL) {
			zend_update_property(default_exception_ce, exception, "previous", sizeof("previous")-1, add_previous TSRMLS_CC);
			Z_DELREF_P(add_previous);
			return;
		}
		exception = previous;
	}
}