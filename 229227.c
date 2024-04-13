PHP_FUNCTION(enchant_broker_list_dicts)
{
	zval *broker;
	enchant_broker *pbroker;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &broker) == FAILURE) {
		RETURN_FALSE;
	}

	PHP_ENCHANT_GET_BROKER;

	enchant_broker_list_dicts(pbroker->pbroker, php_enchant_list_dicts_fn, (void *)return_value);
}