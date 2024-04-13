static zend_bool php_auto_globals_create_server(zend_string *name)
{
	if (PG(variables_order) && (strchr(PG(variables_order),'S') || strchr(PG(variables_order),'s'))) {
		php_register_server_variables();

		if (PG(register_argc_argv)) {
			if (SG(request_info).argc) {
				zval *argc, *argv;

				if ((argc = zend_hash_str_find_ind(&EG(symbol_table), "argc", sizeof("argc")-1)) != NULL &&
					(argv = zend_hash_str_find_ind(&EG(symbol_table), "argv", sizeof("argv")-1)) != NULL) {
					Z_ADDREF_P(argv);
					zend_hash_str_update(Z_ARRVAL(PG(http_globals)[TRACK_VARS_SERVER]), "argv", sizeof("argv")-1, argv);
					zend_hash_str_update(Z_ARRVAL(PG(http_globals)[TRACK_VARS_SERVER]), "argc", sizeof("argc")-1, argc);
				}
			} else {
				php_build_argv(SG(request_info).query_string, &PG(http_globals)[TRACK_VARS_SERVER]);
			}
		}

	} else {
		zval_ptr_dtor(&PG(http_globals)[TRACK_VARS_SERVER]);
		array_init(&PG(http_globals)[TRACK_VARS_SERVER]);
	}

	check_http_proxy(Z_ARRVAL(PG(http_globals)[TRACK_VARS_SERVER]));
	zend_hash_update(&EG(symbol_table), name, &PG(http_globals)[TRACK_VARS_SERVER]);
	Z_ADDREF(PG(http_globals)[TRACK_VARS_SERVER]);

	/* TODO: TRACK_VARS_SERVER is modified in a number of places (e.g. phar) past this point,
	 * where rc>1 due to the $_SERVER global. Ideally this shouldn't happen, but for now we
	 * ignore this issue, as it would probably require larger changes. */
	HT_ALLOW_COW_VIOLATION(Z_ARRVAL(PG(http_globals)[TRACK_VARS_SERVER]));

	return 0; /* don't rearm */
}