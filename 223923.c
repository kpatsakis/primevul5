ZEND_METHOD(exception, getPrevious)
{
	zval *previous;

	DEFAULT_0_PARAMS;

	previous = zend_read_property(default_exception_ce, getThis(), "previous", sizeof("previous")-1, 1 TSRMLS_CC);
	RETURN_ZVAL(previous, 1, 0);
}