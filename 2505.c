static GF_Err gf_text_process_swf(GF_Filter *filter, GF_TXTIn *ctx)
{
	GF_Err e=GF_OK;

	if (!ctx->is_setup) {
		ctx->is_setup = GF_TRUE;
		return gf_text_swf_setup(filter, ctx);
	}
	if (!ctx->opid) return GF_NOT_SUPPORTED;

	if (ctx->seek_state==1) {
		ctx->seek_state = 2;
		gf_swf_reader_del(ctx->swf_parse);
		ctx->swf_parse = gf_swf_reader_new(NULL, ctx->file_name);
		gf_swf_read_header(ctx->swf_parse);
		gf_swf_reader_set_user_mode(ctx->swf_parse, ctx, swf_svg_add_iso_sample, swf_svg_add_iso_header);
	}

	ctx->do_suspend = GF_FALSE;
	/*parse all tags*/
	while (e == GF_OK) {
		e = swf_parse_tag(ctx->swf_parse);
		if (ctx->do_suspend) return GF_OK;
	}
	if (e==GF_EOS) {
		if (ctx->swf_parse->finalize) {
			ctx->swf_parse->finalize(ctx->swf_parse);
			ctx->swf_parse->finalize = NULL;
		}
	}
	return e;
}