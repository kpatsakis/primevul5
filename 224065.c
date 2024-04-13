PHPAPI void php_register_variable(char *var, char *strval, zval *track_vars_array)
{
	php_register_variable_safe(var, strval, strlen(strval), track_vars_array);
}