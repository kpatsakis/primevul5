ZEND_API void zend_throw_exception_object(zval *exception TSRMLS_DC) /* {{{ */
{
	zend_class_entry *exception_ce;

	if (exception == NULL || Z_TYPE_P(exception) != IS_OBJECT) {
		zend_error(E_ERROR, "Need to supply an object when throwing an exception");
	}

	exception_ce = Z_OBJCE_P(exception);

	if (!exception_ce || !instanceof_function(exception_ce, default_exception_ce TSRMLS_CC)) {
		zend_error(E_ERROR, "Exceptions must be valid objects derived from the Exception base class");
	}
	zend_throw_exception_internal(exception TSRMLS_CC);
}