PHPAPI void php_register_variable_ex(char *var_name, zval *val, zval *track_vars_array)
{
	char *p = NULL;
	char *ip = NULL;		/* index pointer */
	char *index;
	char *var, *var_orig;
	size_t var_len, index_len;
	zval gpc_element, *gpc_element_p;
	zend_bool is_array = 0;
	HashTable *symtable1 = NULL;
	ALLOCA_FLAG(use_heap)

	assert(var_name != NULL);

	if (track_vars_array && Z_TYPE_P(track_vars_array) == IS_ARRAY) {
		symtable1 = Z_ARRVAL_P(track_vars_array);
	}

	if (!symtable1) {
		/* Nothing to do */
		zval_dtor(val);
		return;
	}


	/* ignore leading spaces in the variable name */
	while (*var_name==' ') {
		var_name++;
	}

	/*
	 * Prepare variable name
	 */
	var_len = strlen(var_name);
	var = var_orig = do_alloca(var_len + 1, use_heap);
	memcpy(var_orig, var_name, var_len + 1);

	/* ensure that we don't have spaces or dots in the variable name (not binary safe) */
	for (p = var; *p; p++) {
		if (*p == ' ' || *p == '.') {
			*p='_';
		} else if (*p == '[') {
			is_array = 1;
			ip = p;
			*p = 0;
			break;
		}
	}
	var_len = p - var;

	if (var_len==0) { /* empty variable name, or variable name with a space in it */
		zval_dtor(val);
		free_alloca(var_orig, use_heap);
		return;
	}

	if (var_len == sizeof("this")-1 && EG(current_execute_data)) {
		zend_execute_data *ex = EG(current_execute_data);

		while (ex) {
			if (ex->func && ZEND_USER_CODE(ex->func->common.type)) {
				if ((ZEND_CALL_INFO(ex) & ZEND_CALL_HAS_SYMBOL_TABLE)
						&& ex->symbol_table == symtable1) {
					if (memcmp(var, "this", sizeof("this")-1) == 0) {
						zend_throw_error(NULL, "Cannot re-assign $this");
						zval_dtor(val);
						free_alloca(var_orig, use_heap);
						return;
					}
				}
				break;
			}
			ex = ex->prev_execute_data;
		}
	}

	/* GLOBALS hijack attempt, reject parameter */
	if (symtable1 == &EG(symbol_table) &&
		var_len == sizeof("GLOBALS")-1 &&
		!memcmp(var, "GLOBALS", sizeof("GLOBALS")-1)) {
		zval_dtor(val);
		free_alloca(var_orig, use_heap);
		return;
	}

	index = var;
	index_len = var_len;

	if (is_array) {
		int nest_level = 0;
		while (1) {
			char *index_s;
			size_t new_idx_len = 0;

			if(++nest_level > PG(max_input_nesting_level)) {
				HashTable *ht;
				/* too many levels of nesting */

				if (track_vars_array) {
					ht = Z_ARRVAL_P(track_vars_array);
					zend_symtable_str_del(ht, var, var_len);
				}

				zval_dtor(val);

				/* do not output the error message to the screen,
				 this helps us to to avoid "information disclosure" */
				if (!PG(display_errors)) {
					php_error_docref(NULL, E_WARNING, "Input variable nesting level exceeded " ZEND_LONG_FMT ". To increase the limit change max_input_nesting_level in php.ini.", PG(max_input_nesting_level));
				}
				free_alloca(var_orig, use_heap);
				return;
			}

			ip++;
			index_s = ip;
			if (isspace(*ip)) {
				ip++;
			}
			if (*ip==']') {
				index_s = NULL;
			} else {
				ip = strchr(ip, ']');
				if (!ip) {
					/* PHP variables cannot contain '[' in their names, so we replace the character with a '_' */
					*(index_s - 1) = '_';

					index_len = 0;
					if (index) {
						index_len = strlen(index);
					}
					goto plain_var;
					return;
				}
				*ip = 0;
				new_idx_len = strlen(index_s);
			}

			if (!index) {
				array_init(&gpc_element);
				if ((gpc_element_p = zend_hash_next_index_insert(symtable1, &gpc_element)) == NULL) {
					zval_ptr_dtor(&gpc_element);
					zval_dtor(val);
					free_alloca(var_orig, use_heap);
					return;
				}
			} else {
				gpc_element_p = zend_symtable_str_find(symtable1, index, index_len);
				if (!gpc_element_p) {
					zval tmp;
					array_init(&tmp);
					gpc_element_p = zend_symtable_str_update_ind(symtable1, index, index_len, &tmp);
				} else {
					if (Z_TYPE_P(gpc_element_p) == IS_INDIRECT) {
						gpc_element_p = Z_INDIRECT_P(gpc_element_p);
					}
					if (Z_TYPE_P(gpc_element_p) != IS_ARRAY) {
						zval_ptr_dtor(gpc_element_p);
						array_init(gpc_element_p);
					} else {
						SEPARATE_ARRAY(gpc_element_p);
					}
				}
			}
			symtable1 = Z_ARRVAL_P(gpc_element_p);
			/* ip pointed to the '[' character, now obtain the key */
			index = index_s;
			index_len = new_idx_len;

			ip++;
			if (*ip == '[') {
				is_array = 1;
				*ip = 0;
			} else {
				goto plain_var;
			}
		}
	} else {
plain_var:
		ZVAL_COPY_VALUE(&gpc_element, val);
		if (!index) {
			if ((gpc_element_p = zend_hash_next_index_insert(symtable1, &gpc_element)) == NULL) {
				zval_ptr_dtor(&gpc_element);
			}
		} else {
			/*
			 * According to rfc2965, more specific paths are listed above the less specific ones.
			 * If we encounter a duplicate cookie name, we should skip it, since it is not possible
			 * to have the same (plain text) cookie name for the same path and we should not overwrite
			 * more specific cookies with the less specific ones.
			 */
			if (Z_TYPE(PG(http_globals)[TRACK_VARS_COOKIE]) != IS_UNDEF &&
				symtable1 == Z_ARRVAL(PG(http_globals)[TRACK_VARS_COOKIE]) &&
				zend_symtable_str_exists(symtable1, index, index_len)) {
				zval_ptr_dtor(&gpc_element);
			} else {
				gpc_element_p = zend_symtable_str_update_ind(symtable1, index, index_len, &gpc_element);
			}
		}
	}
	free_alloca(var_orig, use_heap);
}