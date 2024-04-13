PHPAPI void php_build_argv(char *s, zval *track_vars_array)
{
	zval arr, argc, tmp;
	int count = 0;
	char *ss, *space;

	if (!(SG(request_info).argc || track_vars_array)) {
		return;
	}

	array_init(&arr);

	/* Prepare argv */
	if (SG(request_info).argc) { /* are we in cli sapi? */
		int i;
		for (i = 0; i < SG(request_info).argc; i++) {
			ZVAL_STRING(&tmp, SG(request_info).argv[i]);
			if (zend_hash_next_index_insert(Z_ARRVAL(arr), &tmp) == NULL) {
				zend_string_free(Z_STR(tmp));
			}
		}
	} else 	if (s && *s) {
		ss = s;
		while (ss) {
			space = strchr(ss, '+');
			if (space) {
				*space = '\0';
			}
			/* auto-type */
			ZVAL_STRING(&tmp, ss);
			count++;
			if (zend_hash_next_index_insert(Z_ARRVAL(arr), &tmp) == NULL) {
				zend_string_free(Z_STR(tmp));
			}
			if (space) {
				*space = '+';
				ss = space + 1;
			} else {
				ss = space;
			}
		}
	}

	/* prepare argc */
	if (SG(request_info).argc) {
		ZVAL_LONG(&argc, SG(request_info).argc);
	} else {
		ZVAL_LONG(&argc, count);
	}

	if (SG(request_info).argc) {
		Z_ADDREF(arr);
		zend_hash_str_update(&EG(symbol_table), "argv", sizeof("argv")-1, &arr);
		zend_hash_str_add(&EG(symbol_table), "argc", sizeof("argc")-1, &argc);
	}
	if (track_vars_array && Z_TYPE_P(track_vars_array) == IS_ARRAY) {
		Z_ADDREF(arr);
		zend_hash_str_update(Z_ARRVAL_P(track_vars_array), "argv", sizeof("argv")-1, &arr);
		zend_hash_str_update(Z_ARRVAL_P(track_vars_array), "argc", sizeof("argc")-1, &argc);
	}
	zval_ptr_dtor_nogc(&arr);
}