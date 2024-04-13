void auth_request_var_expand(string_t *dest, const char *str,
			     const struct auth_request *auth_request,
			     auth_request_escape_func_t *escape_func)
{
	auth_request_var_expand_with_table(dest, str, auth_request,
		auth_request_get_var_expand_table(auth_request, escape_func),
		escape_func);
}