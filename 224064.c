static zend_bool php_auto_globals_create_cookie(zend_string *name)
{
	if (PG(variables_order) && (strchr(PG(variables_order),'C') || strchr(PG(variables_order),'c'))) {
		sapi_module.treat_data(PARSE_COOKIE, NULL, NULL);
	} else {
		zval_ptr_dtor(&PG(http_globals)[TRACK_VARS_COOKIE]);
		array_init(&PG(http_globals)[TRACK_VARS_COOKIE]);
	}

	zend_hash_update(&EG(symbol_table), name, &PG(http_globals)[TRACK_VARS_COOKIE]);
	Z_ADDREF(PG(http_globals)[TRACK_VARS_COOKIE]);

	return 0; /* don't rearm */
}