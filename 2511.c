static GF_Err txtin_process_webvtt(GF_Filter *filter, GF_TXTIn *ctx)
{
	GF_Err e;

	if (!ctx->is_setup) {
		ctx->is_setup = GF_TRUE;
		return txtin_webvtt_setup(filter, ctx);
	}
	if (!ctx->vttparser) return GF_NOT_SUPPORTED;
	if (ctx->seek_state==1) {
		ctx->seek_state = 2;
		gf_webvtt_parser_restart(ctx->vttparser);
	}

	e = gf_webvtt_parser_parse(ctx->vttparser);
	if (e < GF_OK) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[TXTIn] WebVTT process error %s\n", gf_error_to_string(e) ));
	}
	return e;
}