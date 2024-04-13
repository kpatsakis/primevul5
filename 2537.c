static GF_Err txtin_webvtt_setup(GF_Filter *filter, GF_TXTIn *ctx)
{
	GF_Err e;
	u32 ID, OCR_ES_ID, file_size, w, h;
	Bool is_srt;
	char *ext;

	ctx->src = gf_fopen(ctx->file_name, "rb");
	if (!ctx->src) return GF_URL_ERROR;

	file_size = (u32) gf_fsize(ctx->src);

	ctx->unicode_type = gf_text_get_utf_type(ctx->src);
	if (ctx->unicode_type<0) {
		gf_fclose(ctx->src);
		ctx->src = NULL;
		GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[TXTIn] Unsupported SRT UTF encoding\n"));
		return GF_NOT_SUPPORTED;
	}
	ext = gf_file_ext_start(ctx->file_name);
	is_srt = (ext && !strnicmp(ext, ".srt", 4)) ? GF_TRUE : GF_FALSE;


	if (!ctx->timescale) ctx->timescale = 1000;
	OCR_ES_ID = ID = 0;

	if (!ctx->opid) ctx->opid = gf_filter_pid_new(filter);
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_STREAM_TYPE, &PROP_UINT(GF_STREAM_TEXT) );
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_CODECID, &PROP_UINT(GF_CODECID_WEBVTT) );
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_TIMESCALE, &PROP_UINT(ctx->timescale) );
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_DOWN_SIZE, &PROP_LONGUINT(file_size) );

	w = ctx->width;
	h = ctx->height;
	if (!ID) ID = 1;
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_ID, &PROP_UINT(ID) );
	if (OCR_ES_ID) gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_CLOCK_ID, &PROP_UINT(OCR_ES_ID) );
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_WIDTH, &PROP_UINT(w) );
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_HEIGHT, &PROP_UINT(h) );
	if (ctx->zorder) gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_ZORDER, &PROP_SINT(ctx->zorder) );
	if (ctx->lang) gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_LANGUAGE, &PROP_STRING( ctx->lang) );

	ctx->vttparser = gf_webvtt_parser_new();

	e = gf_webvtt_parser_init(ctx->vttparser, ctx->src, ctx->unicode_type, is_srt, ctx, gf_webvtt_import_report, gf_webvtt_flush_sample, gf_webvtt_import_header);
	if (e != GF_OK) {
		gf_webvtt_parser_del(ctx->vttparser);
		ctx->vttparser = NULL;
		GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[TXTIn] WebVTT parser init error %s\n", gf_error_to_string(e) ));
	}
	//get the header
	e = gf_webvtt_parser_parse(ctx->vttparser);

	txtin_probe_duration(ctx);
	return e;
}