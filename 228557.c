PHPAPI MYSQLND_PROTOCOL *
mysqlnd_protocol_init(zend_bool persistent TSRMLS_DC)
{
	MYSQLND_PROTOCOL * ret;
	DBG_ENTER("mysqlnd_protocol_init");
	ret = MYSQLND_CLASS_METHOD_TABLE_NAME(mysqlnd_object_factory).get_protocol_decoder(persistent TSRMLS_CC);
	DBG_RETURN(ret);