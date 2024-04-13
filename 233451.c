static PHP_MSHUTDOWN_FUNCTION(libxml)
{
	php_libxml_shutdown();

	return SUCCESS;
}