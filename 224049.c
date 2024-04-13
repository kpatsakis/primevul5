static zend_bool php_auto_globals_create_post(zend_string *name)
{
	if (PG(variables_order) &&
			(strchr(PG(variables_order),'P') || strchr(PG(variables_order),'p')) &&
		!SG(headers_sent) &&
		SG(request_info).request_method &&
		!strcasecmp(SG(request_info).request_method, "POST")) {
		sapi_module.treat_data(PARSE_POST, NULL, NULL);
	} else {
		zval_ptr_dtor(&PG(http_globals)[TRACK_VARS_POST]);
		array_init(&PG(http_globals)[TRACK_VARS_POST]);
	}

	zend_hash_update(&EG(symbol_table), name, &PG(http_globals)[TRACK_VARS_POST]);
	Z_ADDREF(PG(http_globals)[TRACK_VARS_POST]);

	return 0; /* don't rearm */
}