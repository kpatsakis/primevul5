void auth_request_var_expand_with_table(string_t *dest, const char *str,
					const struct auth_request *auth_request,
					const struct var_expand_table *table,
					auth_request_escape_func_t *escape_func)
{
	struct auth_request_var_expand_ctx ctx;

	memset(&ctx, 0, sizeof(ctx));
	ctx.auth_request = auth_request;
	ctx.escape_func = escape_func == NULL ? escape_none : escape_func;
	var_expand_with_funcs(dest, str, table,
			      auth_request_var_funcs_table, &ctx);
}