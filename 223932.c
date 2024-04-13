void zend_register_default_exception(TSRMLS_D) /* {{{ */
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "Exception", default_exception_functions);
	default_exception_ce = zend_register_internal_class(&ce TSRMLS_CC);
	default_exception_ce->create_object = zend_default_exception_new;
	memcpy(&default_exception_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	default_exception_handlers.clone_obj = NULL;

	zend_declare_property_string(default_exception_ce, "message", sizeof("message")-1, "", ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_string(default_exception_ce, "string", sizeof("string")-1, "", ZEND_ACC_PRIVATE TSRMLS_CC);
	zend_declare_property_long(default_exception_ce, "code", sizeof("code")-1, 0, ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(default_exception_ce, "file", sizeof("file")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(default_exception_ce, "line", sizeof("line")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(default_exception_ce, "trace", sizeof("trace")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
	zend_declare_property_null(default_exception_ce, "previous", sizeof("previous")-1, ZEND_ACC_PRIVATE TSRMLS_CC);

	INIT_CLASS_ENTRY(ce, "ErrorException", error_exception_functions);
	error_exception_ce = zend_register_internal_class_ex(&ce, default_exception_ce, NULL TSRMLS_CC);
	error_exception_ce->create_object = zend_error_exception_new;
	zend_declare_property_long(error_exception_ce, "severity", sizeof("severity")-1, E_ERROR, ZEND_ACC_PROTECTED TSRMLS_CC);
}