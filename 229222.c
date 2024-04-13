PHP_MINIT_FUNCTION(enchant)
{
	le_enchant_broker = zend_register_list_destructors_ex(php_enchant_broker_free, NULL, "enchant_broker", module_number);
	le_enchant_dict = zend_register_list_destructors_ex(php_enchant_dict_free, NULL, "enchant_dict", module_number);
	REGISTER_LONG_CONSTANT("ENCHANT_MYSPELL", PHP_ENCHANT_MYSPELL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ENCHANT_ISPELL", PHP_ENCHANT_ISPELL, CONST_CS | CONST_PERSISTENT);
	return SUCCESS;
}