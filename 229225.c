PHP_FUNCTION(enchant_broker_get_dict_path)
{
	zval *broker;
	enchant_broker *pbroker;
	long dict_type;
	char *value;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &broker, &dict_type) == FAILURE) {
		RETURN_FALSE;
	}
	
	PHP_ENCHANT_GET_BROKER;

	switch (dict_type) {
		case PHP_ENCHANT_MYSPELL:
			PHP_ENCHANT_GET_BROKER;
			value = enchant_broker_get_param(pbroker->pbroker, "enchant.myspell.dictionary.path");
			break;

		case PHP_ENCHANT_ISPELL:
			PHP_ENCHANT_GET_BROKER;
			value = enchant_broker_get_param(pbroker->pbroker, "enchant.ispell.dictionary.path");
			break;

		default:
			RETURN_FALSE;
	}

	RETURN_STRING(value, 1);
}