static void gf_webvtt_import_header(void *user, const char *config)
{
	GF_TXTIn *ctx = (GF_TXTIn *)user;
	if (!ctx->hdr_parsed) {
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_DECODER_CONFIG, &PROP_DATA((char *) config, (u32) (1+strlen(config)) ) );
		ctx->hdr_parsed = GF_TRUE;
		gf_webvtt_parser_suspend(ctx->vttparser);
	}
}