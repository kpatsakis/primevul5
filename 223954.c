static zend_object_value zend_default_exception_new(zend_class_entry *class_type TSRMLS_DC) /* {{{ */
{
	return zend_default_exception_new_ex(class_type, 0 TSRMLS_CC);
}