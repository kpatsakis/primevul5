static
size_t php_mysqlnd_sha256_pk_request_write(void * _packet, MYSQLND_CONN_DATA * conn TSRMLS_DC)
{
	zend_uchar buffer[MYSQLND_HEADER_SIZE + 1];
	size_t sent;

	DBG_ENTER("php_mysqlnd_sha256_pk_request_write");

	int1store(buffer + MYSQLND_HEADER_SIZE, '\1');
	sent = conn->net->data->m.send_ex(conn->net, buffer, 1, conn->stats, conn->error_info TSRMLS_CC);

	DBG_RETURN(sent);