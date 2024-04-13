static zend_bool php_auto_globals_create_request(zend_string *name)
{
	zval form_variables;
	unsigned char _gpc_flags[3] = {0, 0, 0};
	char *p;

	array_init(&form_variables);

	if (PG(request_order) != NULL) {
		p = PG(request_order);
	} else {
		p = PG(variables_order);
	}

	for (; p && *p; p++) {
		switch (*p) {
			case 'g':
			case 'G':
				if (!_gpc_flags[0]) {
					php_autoglobal_merge(Z_ARRVAL(form_variables), Z_ARRVAL(PG(http_globals)[TRACK_VARS_GET]));
					_gpc_flags[0] = 1;
				}
				break;
			case 'p':
			case 'P':
				if (!_gpc_flags[1]) {
					php_autoglobal_merge(Z_ARRVAL(form_variables), Z_ARRVAL(PG(http_globals)[TRACK_VARS_POST]));
					_gpc_flags[1] = 1;
				}
				break;
			case 'c':
			case 'C':
				if (!_gpc_flags[2]) {
					php_autoglobal_merge(Z_ARRVAL(form_variables), Z_ARRVAL(PG(http_globals)[TRACK_VARS_COOKIE]));
					_gpc_flags[2] = 1;
				}
				break;
		}
	}

	zend_hash_update(&EG(symbol_table), name, &form_variables);
	return 0;
}