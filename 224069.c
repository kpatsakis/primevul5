PHPAPI int php_hash_environment(void)
{
	memset(PG(http_globals), 0, sizeof(PG(http_globals)));
	zend_activate_auto_globals();
	if (PG(register_argc_argv)) {
		php_build_argv(SG(request_info).query_string, &PG(http_globals)[TRACK_VARS_SERVER]);
	}
	return SUCCESS;
}