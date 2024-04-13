ZEND_METHOD(exception, __toString)
{
	zval message, file, line, *trace, *exception;
	char *str, *prev_str;
	int len = 0;
	zend_fcall_info fci;
	zval fname;
	
	DEFAULT_0_PARAMS;
	
	str = estrndup("", 0);

	exception = getThis();
	ZVAL_STRINGL(&fname, "gettraceasstring", sizeof("gettraceasstring")-1, 1);

	while (exception && Z_TYPE_P(exception) == IS_OBJECT) {
		prev_str = str;
		_default_exception_get_entry(exception, "message", sizeof("message")-1, &message TSRMLS_CC);
		_default_exception_get_entry(exception, "file", sizeof("file")-1, &file TSRMLS_CC);
		_default_exception_get_entry(exception, "line", sizeof("line")-1, &line TSRMLS_CC);

		convert_to_string(&message);
		convert_to_string(&file);
		convert_to_long(&line);

		fci.size = sizeof(fci);
		fci.function_table = &Z_OBJCE_P(exception)->function_table;
		fci.function_name = &fname;
		fci.symbol_table = NULL;
		fci.object_ptr = exception;
		fci.retval_ptr_ptr = &trace;
		fci.param_count = 0;
		fci.params = NULL;
		fci.no_separation = 1;

		zend_call_function(&fci, NULL TSRMLS_CC);

		if (Z_TYPE_P(trace) != IS_STRING) {
			zval_ptr_dtor(&trace);
			trace = NULL;
		}

		if (Z_STRLEN(message) > 0) {
			len = zend_spprintf(&str, 0, "exception '%s' with message '%s' in %s:%ld\nStack trace:\n%s%s%s",
								Z_OBJCE_P(exception)->name, Z_STRVAL(message), Z_STRVAL(file), Z_LVAL(line),
								(trace && Z_STRLEN_P(trace)) ? Z_STRVAL_P(trace) : "#0 {main}\n",
								len ? "\n\nNext " : "", prev_str);
		} else {
			len = zend_spprintf(&str, 0, "exception '%s' in %s:%ld\nStack trace:\n%s%s%s",
								Z_OBJCE_P(exception)->name, Z_STRVAL(file), Z_LVAL(line),
								(trace && Z_STRLEN_P(trace)) ? Z_STRVAL_P(trace) : "#0 {main}\n",
								len ? "\n\nNext " : "", prev_str);
		}
		efree(prev_str);
		zval_dtor(&message);
		zval_dtor(&file);
		zval_dtor(&line);

		exception = zend_read_property(default_exception_ce, exception, "previous", sizeof("previous")-1, 0 TSRMLS_CC);

		if (trace) {
			zval_ptr_dtor(&trace);
		}

	}
	zval_dtor(&fname);

	/* We store the result in the private property string so we can access
	 * the result in uncaught exception handlers without memleaks. */
	zend_update_property_string(default_exception_ce, getThis(), "string", sizeof("string")-1, str TSRMLS_CC);

	RETURN_STRINGL(str, len, 0);
}