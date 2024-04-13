static struct st_mysqlnd_packet_sha256_pk_request_response *
MYSQLND_METHOD(mysqlnd_protocol, get_sha256_pk_request_response_packet)(MYSQLND_PROTOCOL * const protocol, zend_bool persistent TSRMLS_DC)
{
	struct st_mysqlnd_packet_sha256_pk_request_response * packet = mnd_pecalloc(1, packet_methods[PROT_SHA256_PK_REQUEST_RESPONSE_PACKET].struct_size, persistent);
	DBG_ENTER("mysqlnd_protocol::get_sha256_pk_request_response_packet");
	if (packet) {
		packet->header.m = &packet_methods[PROT_SHA256_PK_REQUEST_RESPONSE_PACKET];
		packet->header.persistent = persistent;
	}
	DBG_RETURN(packet);