static inline void php_register_server_variables(void)
{
	zval request_time_float, request_time_long;

	zval_ptr_dtor(&PG(http_globals)[TRACK_VARS_SERVER]);
	array_init(&PG(http_globals)[TRACK_VARS_SERVER]);

	/* Server variables */
	if (sapi_module.register_server_variables) {
		sapi_module.register_server_variables(&PG(http_globals)[TRACK_VARS_SERVER]);
	}

	/* PHP Authentication support */
	if (SG(request_info).auth_user) {
		php_register_variable("PHP_AUTH_USER", SG(request_info).auth_user, &PG(http_globals)[TRACK_VARS_SERVER]);
	}
	if (SG(request_info).auth_password) {
		php_register_variable("PHP_AUTH_PW", SG(request_info).auth_password, &PG(http_globals)[TRACK_VARS_SERVER]);
	}
	if (SG(request_info).auth_digest) {
		php_register_variable("PHP_AUTH_DIGEST", SG(request_info).auth_digest, &PG(http_globals)[TRACK_VARS_SERVER]);
	}

	/* store request init time */
	ZVAL_DOUBLE(&request_time_float, sapi_get_request_time());
	php_register_variable_ex("REQUEST_TIME_FLOAT", &request_time_float, &PG(http_globals)[TRACK_VARS_SERVER]);
	ZVAL_LONG(&request_time_long, zend_dval_to_lval(Z_DVAL(request_time_float)));
	php_register_variable_ex("REQUEST_TIME", &request_time_long, &PG(http_globals)[TRACK_VARS_SERVER]);
}