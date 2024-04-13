void php_startup_auto_globals(void)
{
	zend_register_auto_global(zend_string_init("_GET", sizeof("_GET")-1, 1), 0, php_auto_globals_create_get);
	zend_register_auto_global(zend_string_init("_POST", sizeof("_POST")-1, 1), 0, php_auto_globals_create_post);
	zend_register_auto_global(zend_string_init("_COOKIE", sizeof("_COOKIE")-1, 1), 0, php_auto_globals_create_cookie);
	zend_register_auto_global(zend_string_init("_SERVER", sizeof("_SERVER")-1, 1), PG(auto_globals_jit), php_auto_globals_create_server);
	zend_register_auto_global(zend_string_init("_ENV", sizeof("_ENV")-1, 1), PG(auto_globals_jit), php_auto_globals_create_env);
	zend_register_auto_global(zend_string_init("_REQUEST", sizeof("_REQUEST")-1, 1), PG(auto_globals_jit), php_auto_globals_create_request);
	zend_register_auto_global(zend_string_init("_FILES", sizeof("_FILES")-1, 1), 0, php_auto_globals_create_files);
}