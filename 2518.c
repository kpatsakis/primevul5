static GF_Err gf_text_swf_setup(GF_Filter *filter, GF_TXTIn *ctx)
{
	GF_Err e;
	u32 ID;

	ctx->swf_parse = gf_swf_reader_new(NULL, ctx->file_name);
	e = gf_swf_read_header(ctx->swf_parse);
	if (e) return e;
	gf_swf_reader_set_user_mode(ctx->swf_parse, ctx, swf_svg_add_iso_sample, swf_svg_add_iso_header);

	if (!ctx->timescale) ctx->timescale = 1000;

	if (!ctx->opid) ctx->opid = gf_filter_pid_new(filter);
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_STREAM_TYPE, &PROP_UINT(GF_STREAM_TEXT) );
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_CODECID, &PROP_UINT(GF_CODECID_SIMPLE_TEXT) );
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_TIMESCALE, &PROP_UINT(ctx->timescale) );
//	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_DOWN_SIZE, &PROP_UINT(file_size) );

	//patch for old arch
	ctx->width = FIX2INT(ctx->swf_parse->width);
	ctx->height = FIX2INT(ctx->swf_parse->height);
	if (!ctx->width && !ctx->height) {
		ctx->width = 400;
		ctx->height = 60;
	}
	ID = 1;
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_ID, &PROP_UINT(ID) );
	if (ctx->width) gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_WIDTH, &PROP_UINT(ctx->width) );
	if (ctx->height) gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_HEIGHT, &PROP_UINT(ctx->height) );
	if (ctx->zorder) gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_ZORDER, &PROP_SINT(ctx->zorder) );
	if (ctx->lang) gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_LANGUAGE, &PROP_STRING( ctx->lang) );

	gf_filter_pid_set_property_str(ctx->opid, "meta:mime", &PROP_STRING("image/svg+xml") );

#ifndef GPAC_DISABLE_SVG
	GF_LOG(GF_LOG_WARNING, GF_LOG_PARSER, ("[TXTIn] swf -> svg not fully migrated, using SWF flags 0 and no flatten angle. Patch welcome\n"));
	e = swf_to_svg_init(ctx->swf_parse, 0, 0);
#endif

	//SWF->BIFS is handled in ctx loader, no need to define it here
	txtin_probe_duration(ctx);

	return e;
}