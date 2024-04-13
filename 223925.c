ZEND_API zval * zend_throw_error_exception(zend_class_entry *exception_ce, char *message, long code, int severity TSRMLS_DC) /* {{{ */
{
	zval *ex = zend_throw_exception(exception_ce, message, code TSRMLS_CC);
	zend_update_property_long(default_exception_ce, ex, "severity", sizeof("severity")-1, severity TSRMLS_CC);
	return ex;
}