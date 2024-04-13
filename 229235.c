PHP_FUNCTION(enchant_broker_free)
{
	zval *broker;
	enchant_broker *pbroker;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &broker) == FAILURE) {
		RETURN_FALSE;
	}
	PHP_ENCHANT_GET_BROKER;

	zend_list_delete(Z_RESVAL_P(broker));
	RETURN_TRUE;
}