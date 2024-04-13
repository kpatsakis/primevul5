static zend_bool php_auto_globals_create_env(zend_string *name)
{
	zval_ptr_dtor(&PG(http_globals)[TRACK_VARS_ENV]);
	array_init(&PG(http_globals)[TRACK_VARS_ENV]);

	if (PG(variables_order) && (strchr(PG(variables_order),'E') || strchr(PG(variables_order),'e'))) {
		php_import_environment_variables(&PG(http_globals)[TRACK_VARS_ENV]);
	}

	check_http_proxy(Z_ARRVAL(PG(http_globals)[TRACK_VARS_ENV]));
	zend_hash_update(&EG(symbol_table), name, &PG(http_globals)[TRACK_VARS_ENV]);
	Z_ADDREF(PG(http_globals)[TRACK_VARS_ENV]);

	return 0; /* don't rearm */
}