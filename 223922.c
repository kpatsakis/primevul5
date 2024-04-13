ZEND_METHOD(error_exception, getSeverity)
{
	DEFAULT_0_PARAMS;

	_default_exception_get_entry(getThis(), "severity", sizeof("severity")-1, return_value TSRMLS_CC);
}