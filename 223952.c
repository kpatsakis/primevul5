static zend_object_value zend_default_exception_new_ex(zend_class_entry *class_type, int skip_top_traces TSRMLS_DC) /* {{{ */
{
	zval obj;
	zend_object *object;
	zval *trace;

	Z_OBJVAL(obj) = zend_objects_new(&object, class_type TSRMLS_CC);
	Z_OBJ_HT(obj) = &default_exception_handlers;

	object_properties_init(object, class_type);

	ALLOC_ZVAL(trace);
	Z_UNSET_ISREF_P(trace);
	Z_SET_REFCOUNT_P(trace, 0);
	zend_fetch_debug_backtrace(trace, skip_top_traces, 0, 0 TSRMLS_CC);

	zend_update_property_string(default_exception_ce, &obj, "file", sizeof("file")-1, zend_get_executed_filename(TSRMLS_C) TSRMLS_CC);
	zend_update_property_long(default_exception_ce, &obj, "line", sizeof("line")-1, zend_get_executed_lineno(TSRMLS_C) TSRMLS_CC);
	zend_update_property(default_exception_ce, &obj, "trace", sizeof("trace")-1, trace TSRMLS_CC);

	return Z_OBJVAL(obj);
}