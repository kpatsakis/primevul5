PHP_FUNCTION(enchant_broker_init)
{
	enchant_broker *broker;
	EnchantBroker *pbroker;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	pbroker = enchant_broker_init();

	if (pbroker) {
		broker = (enchant_broker *) emalloc(sizeof(enchant_broker));
		broker->pbroker = pbroker;
		broker->dict = NULL;
		broker->dictcnt = 0;
		broker->rsrc_id = ZEND_REGISTER_RESOURCE(return_value, broker, le_enchant_broker);
	} else {
		RETURN_FALSE;
	}
}