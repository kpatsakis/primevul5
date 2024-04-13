static void check_http_proxy(HashTable *var_table)
{
	if (zend_hash_str_exists(var_table, "HTTP_PROXY", sizeof("HTTP_PROXY")-1)) {
		char *local_proxy = getenv("HTTP_PROXY");

		if (!local_proxy) {
			zend_hash_str_del(var_table, "HTTP_PROXY", sizeof("HTTP_PROXY")-1);
		} else {
			zval local_zval;
			ZVAL_STRING(&local_zval, local_proxy);
			zend_hash_str_update(var_table, "HTTP_PROXY", sizeof("HTTP_PROXY")-1, &local_zval);
		}
	}
}