static zend_bool php_auto_globals_create_get(zend_string *name)
{
	if (PG(variables_order) && (strchr(PG(variables_order),'G') || strchr(PG(variables_order),'g'))) {
		sapi_module.treat_data(PARSE_GET, NULL, NULL);
	} else {
		zval_ptr_dtor(&PG(http_globals)[TRACK_VARS_GET]);
		array_init(&PG(http_globals)[TRACK_VARS_GET]);
	}

	zend_hash_update(&EG(symbol_table), name, &PG(http_globals)[TRACK_VARS_GET]);
	Z_ADDREF(PG(http_globals)[TRACK_VARS_GET]);

	return 0; /* don't rearm */
}