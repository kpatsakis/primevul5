auth_request_var_expand_func_passdb(const char *data, void *context)
{
	struct auth_request_var_expand_ctx *ctx = context;
	const char *field_name = t_strcut(data, ':');
	const char *value;

	value = auth_fields_find(ctx->auth_request->extra_fields, field_name);
	return ctx->escape_func(value != NULL ? value : field_get_default(data),
				ctx->auth_request);
}