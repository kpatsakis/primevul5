check_spnego_options(spnego_gss_ctx_id_t spnego_ctx)
{
	spnego_ctx->optionStr = gssint_get_modOptions(
		(const gss_OID)&spnego_oids[0]);
}