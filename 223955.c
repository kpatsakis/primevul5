ZEND_API zval * zend_throw_exception_ex(zend_class_entry *exception_ce, long code TSRMLS_DC, char *format, ...) /* {{{ */
{
	va_list arg;
	char *message;
	zval *zexception;

	va_start(arg, format);
	zend_vspprintf(&message, 0, format, arg);
	va_end(arg);
	zexception = zend_throw_exception(exception_ce, message, code TSRMLS_CC);
	efree(message);
	return zexception;
}