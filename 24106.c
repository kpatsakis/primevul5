t_auth_request_var_expand(const char *str,
			  const struct auth_request *auth_request,
			  auth_request_escape_func_t *escape_func)
{
	string_t *dest = t_str_new(128);
	auth_request_var_expand(dest, str, auth_request, escape_func);
	return str_c(dest);
}