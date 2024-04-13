auth_request_get_var_expand_table(const struct auth_request *auth_request,
				  auth_request_escape_func_t *escape_func)
{
	unsigned int count = 0;

	return auth_request_get_var_expand_table_full(auth_request, escape_func,
						      &count);
}