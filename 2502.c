static void ttxtin_reset(GF_TXTIn *ctx)
{
	if (ctx->samp) gf_isom_delete_text_sample(ctx->samp);
	ctx->samp = NULL;
	if (ctx->src) gf_fclose(ctx->src);
	ctx->src = NULL;
	if (ctx->vttparser) gf_webvtt_parser_del(ctx->vttparser);
	ctx->vttparser = NULL;
	if (ctx->parser) gf_xml_dom_del(ctx->parser);
	ctx->parser = NULL;
	if (ctx->parser_working_copy) gf_xml_dom_del(ctx->parser_working_copy);
	ctx->parser_working_copy = NULL;
}