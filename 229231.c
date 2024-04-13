PHP_MINFO_FUNCTION(enchant)
{
	EnchantBroker *pbroker;

	pbroker = enchant_broker_init();
	php_info_print_table_start();
	php_info_print_table_header(2, "enchant support", "enabled");
	php_info_print_table_row(2, "Version", PHP_ENCHANT_VERSION);
#ifdef ENCHANT_VERSION_STRING
	php_info_print_table_row(2, "Libenchant Version", ENCHANT_VERSION_STRING);
#elif defined(HAVE_ENCHANT_BROKER_SET_PARAM)
	php_info_print_table_row(2, "Libenchant Version", "1.5.0 or later");
#endif
	php_info_print_table_row(2, "Revision", "$Id$");
	php_info_print_table_end();

	php_info_print_table_start();
	enchant_broker_describe(pbroker, __enumerate_providers_fn, NULL);
	php_info_print_table_end();
	enchant_broker_free(pbroker);
}