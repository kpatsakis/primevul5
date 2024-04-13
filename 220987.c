inflate_generic_signature_checked (MonoImage *image, MonoMethodSignature *sig, MonoGenericContext *context, MonoError *error)
{
	MonoMethodSignature *res;
	gboolean is_open;
	int i;

	mono_error_init (error);
	if (!context)
		return sig;

	res = g_malloc0 (MONO_SIZEOF_METHOD_SIGNATURE + ((gint32)sig->param_count) * sizeof (MonoType*));
	res->param_count = sig->param_count;
	res->sentinelpos = -1;
	res->ret = mono_class_inflate_generic_type_checked (sig->ret, context, error);
	if (!mono_error_ok (error))
		goto fail;
	is_open = mono_class_is_open_constructed_type (res->ret);
	for (i = 0; i < sig->param_count; ++i) {
		res->params [i] = mono_class_inflate_generic_type_checked (sig->params [i], context, error);
		if (!mono_error_ok (error))
			goto fail;

		if (!is_open)
			is_open = mono_class_is_open_constructed_type (res->params [i]);
	}
	res->hasthis = sig->hasthis;
	res->explicit_this = sig->explicit_this;
	res->call_convention = sig->call_convention;
	res->pinvoke = sig->pinvoke;
	res->generic_param_count = sig->generic_param_count;
	res->sentinelpos = sig->sentinelpos;
	res->has_type_parameters = is_open;
	res->is_inflated = 1;
	return res;

fail:
	if (res->ret)
		mono_metadata_free_type (res->ret);
	for (i = 0; i < sig->param_count; ++i) {
		if (res->params [i])
			mono_metadata_free_type (res->params [i]);
	}
	g_free (res);
	return NULL;
}