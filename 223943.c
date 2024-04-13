ZEND_METHOD(exception, getTrace)
{
	DEFAULT_0_PARAMS;

	_default_exception_get_entry(getThis(), "trace", sizeof("trace")-1, return_value TSRMLS_CC);
}