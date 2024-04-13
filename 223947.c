ZEND_API void zend_exception_error(zval *exception, int severity TSRMLS_DC) /* {{{ */
{
	zend_class_entry *ce_exception = Z_OBJCE_P(exception);
	if (instanceof_function(ce_exception, default_exception_ce TSRMLS_CC)) {
		zval *str, *file, *line;

		EG(exception) = NULL;

		zend_call_method_with_0_params(&exception, ce_exception, NULL, "__tostring", &str);
		if (!EG(exception)) {
			if (Z_TYPE_P(str) != IS_STRING) {
				zend_error(E_WARNING, "%s::__toString() must return a string", ce_exception->name);
			} else {
				zend_update_property_string(default_exception_ce, exception, "string", sizeof("string")-1, EG(exception) ? ce_exception->name : Z_STRVAL_P(str) TSRMLS_CC);
			}
		}
		zval_ptr_dtor(&str);

		if (EG(exception)) {
			/* do the best we can to inform about the inner exception */
			if (instanceof_function(ce_exception, default_exception_ce TSRMLS_CC)) {
				file = zend_read_property(default_exception_ce, EG(exception), "file", sizeof("file")-1, 1 TSRMLS_CC);
				line = zend_read_property(default_exception_ce, EG(exception), "line", sizeof("line")-1, 1 TSRMLS_CC);

				convert_to_string(file);
				file = (Z_STRLEN_P(file) > 0) ? file : NULL;
				line = (Z_TYPE_P(line) == IS_LONG) ? line : NULL;
			} else {
				file = NULL;
				line = NULL;
			}
			zend_error_va(E_WARNING, file ? Z_STRVAL_P(file) : NULL, line ? Z_LVAL_P(line) : 0, "Uncaught %s in exception handling during call to %s::__tostring()", Z_OBJCE_P(EG(exception))->name, ce_exception->name);
		}

		str = zend_read_property(default_exception_ce, exception, "string", sizeof("string")-1, 1 TSRMLS_CC);
		file = zend_read_property(default_exception_ce, exception, "file", sizeof("file")-1, 1 TSRMLS_CC);
		line = zend_read_property(default_exception_ce, exception, "line", sizeof("line")-1, 1 TSRMLS_CC);

		convert_to_string(str);
		convert_to_string(file);
		convert_to_long(line);

		zend_error_va(severity, (Z_STRLEN_P(file) > 0) ? Z_STRVAL_P(file) : NULL, Z_LVAL_P(line), "Uncaught %s\n  thrown", Z_STRVAL_P(str));
	} else {
		zend_error(severity, "Uncaught exception '%s'", ce_exception->name);
	}
}