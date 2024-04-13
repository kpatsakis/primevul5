PHP_FUNCTION(enchant_broker_set_dict_path)
{
	zval *broker;
	enchant_broker *pbroker;
	long dict_type;
	char *value;
	int value_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rls", &broker, &dict_type, &value, &value_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (!value_len) {
		RETURN_FALSE;
	}
	
	PHP_ENCHANT_GET_BROKER;

	switch (dict_type) {
		case PHP_ENCHANT_MYSPELL:
			PHP_ENCHANT_GET_BROKER;
			enchant_broker_set_param(pbroker->pbroker, "enchant.myspell.dictionary.path", (const char *)value);
			RETURN_TRUE;
			break;

		case PHP_ENCHANT_ISPELL:
			PHP_ENCHANT_GET_BROKER;
			enchant_broker_set_param(pbroker->pbroker, "enchant.ispell.dictionary.path", (const char *)value);
			RETURN_TRUE;
			break;

		default:
			RETURN_FALSE;
	}
}