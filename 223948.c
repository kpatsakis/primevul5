static int _build_trace_string(zval **frame TSRMLS_DC, int num_args, va_list args, zend_hash_key *hash_key) /* {{{ */
{
	char *s_tmp, **str;
	int *len, *num;
	long line;
	HashTable *ht = Z_ARRVAL_PP(frame);
	zval **file, **tmp;

	if (Z_TYPE_PP(frame) != IS_ARRAY) {
		zend_error(E_WARNING, "Expected array for frame %lu", hash_key->h);
		return ZEND_HASH_APPLY_KEEP;
	}

	str = va_arg(args, char**);
	len = va_arg(args, int*);
	num = va_arg(args, int*);

	s_tmp = emalloc(1 + MAX_LENGTH_OF_LONG + 1 + 1);
	sprintf(s_tmp, "#%d ", (*num)++);
	TRACE_APPEND_STRL(s_tmp, strlen(s_tmp));
	efree(s_tmp);
	if (zend_hash_find(ht, "file", sizeof("file"), (void**)&file) == SUCCESS) {
		if (Z_TYPE_PP(file) != IS_STRING) {
			zend_error(E_WARNING, "Function name is no string");
			TRACE_APPEND_STR("[unknown function]");
		} else{
			if (zend_hash_find(ht, "line", sizeof("line"), (void**)&tmp) == SUCCESS) {
				if (Z_TYPE_PP(tmp) == IS_LONG) {
					line = Z_LVAL_PP(tmp);
				} else {
					zend_error(E_WARNING, "Line is no long");
					line = 0;
				}
			} else {
				line = 0;
			}
			s_tmp = emalloc(Z_STRLEN_PP(file) + MAX_LENGTH_OF_LONG + 4 + 1);
			sprintf(s_tmp, "%s(%ld): ", Z_STRVAL_PP(file), line);
			TRACE_APPEND_STRL(s_tmp, strlen(s_tmp));
			efree(s_tmp);
		}
	} else {
		TRACE_APPEND_STR("[internal function]: ");
	}
	TRACE_APPEND_KEY("class");
	TRACE_APPEND_KEY("type");
	TRACE_APPEND_KEY("function");
	TRACE_APPEND_CHR('(');
	if (zend_hash_find(ht, "args", sizeof("args"), (void**)&tmp) == SUCCESS) {
		if (Z_TYPE_PP(tmp) == IS_ARRAY) {
			int last_len = *len;
			zend_hash_apply_with_arguments(Z_ARRVAL_PP(tmp) TSRMLS_CC, (apply_func_args_t)_build_trace_args, 2, str, len);
			if (last_len != *len) {
				*len -= 2; /* remove last ', ' */
			}
		} else {
			zend_error(E_WARNING, "args element is no array");
		}
	}
	TRACE_APPEND_STR(")\n");
	return ZEND_HASH_APPLY_KEEP;
}