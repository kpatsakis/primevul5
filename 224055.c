zend_bool php_std_auto_global_callback(char *name, uint32_t name_len)
{
	zend_printf("%s\n", name);
	return 0; /* don't rearm */
}