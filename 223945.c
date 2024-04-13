ZEND_API void zend_clear_exception(TSRMLS_D) /* {{{ */
{
	if (EG(prev_exception)) {
		zval_ptr_dtor(&EG(prev_exception));
		EG(prev_exception) = NULL;
	}
	if (!EG(exception)) {
		return;
	}
	zval_ptr_dtor(&EG(exception));
	EG(exception) = NULL;
	EG(current_execute_data)->opline = EG(opline_before_exception);
#if ZEND_DEBUG
	EG(opline_before_exception) = NULL;
#endif
}