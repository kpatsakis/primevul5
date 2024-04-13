static void
php_mysqlnd_change_auth_response_free_mem(void * _packet, zend_bool stack_allocation TSRMLS_DC)
{
	if (!stack_allocation) {
		MYSQLND_PACKET_CHANGE_AUTH_RESPONSE * p = (MYSQLND_PACKET_CHANGE_AUTH_RESPONSE *) _packet;
		mnd_pefree(p, p->header.persistent);
	}