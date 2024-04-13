static zend_bool php_auto_globals_create_files(zend_string *name)
{
	if (Z_TYPE(PG(http_globals)[TRACK_VARS_FILES]) == IS_UNDEF) {
		array_init(&PG(http_globals)[TRACK_VARS_FILES]);
	}

	zend_hash_update(&EG(symbol_table), name, &PG(http_globals)[TRACK_VARS_FILES]);
	Z_ADDREF(PG(http_globals)[TRACK_VARS_FILES]);

	return 0; /* don't rearm */
}