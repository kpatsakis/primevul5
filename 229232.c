PHP_FUNCTION(enchant_broker_get_error)
{
	zval *broker;
	enchant_broker *pbroker;
	char *msg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &broker) == FAILURE) {
		RETURN_FALSE;
	}

	PHP_ENCHANT_GET_BROKER;

	msg = enchant_broker_get_error(pbroker->pbroker);
	if (msg) {
		RETURN_STRING((char *)msg, 1);
	}
	RETURN_FALSE;
}