release_spnego_ctx(spnego_gss_ctx_id_t *ctx)
{
	spnego_gss_ctx_id_t context;
	OM_uint32 minor_stat;
	context = *ctx;

	if (context != NULL) {
		(void) gss_release_buffer(&minor_stat,
					&context->DER_mechTypes);

		(void) gss_release_oid_set(&minor_stat, &context->mech_set);

		(void) gss_release_name(&minor_stat, &context->internal_name);

		if (context->optionStr != NULL) {
			free(context->optionStr);
			context->optionStr = NULL;
		}
		free(context);
		*ctx = NULL;
	}
}