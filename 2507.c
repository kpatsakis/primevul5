void txtin_finalize(GF_Filter *filter)
{
	GF_TXTIn *ctx = gf_filter_get_udta(filter);

	ttxtin_reset(ctx);
	if (ctx->bs_w) gf_bs_del(ctx->bs_w);

	if (ctx->text_descs) {
		while (gf_list_count(ctx->text_descs)) {
			GF_PropertyValue *p = gf_list_pop_back(ctx->text_descs);
			gf_free(p->value.data.ptr);
			gf_free(p);
		}
		gf_list_del(ctx->text_descs);
	}
#ifndef GPAC_DISABLE_SWF_IMPORT
	gf_swf_reader_del(ctx->swf_parse);
#endif

	if (ctx->intervals) {
		ttml_reset_intervals(ctx);
		gf_list_del(ctx->intervals);
	}
	if (ctx->ttml_resources) {
		while (gf_list_count(ctx->ttml_resources)) {
			TTMLRes *ires = gf_list_pop_back(ctx->ttml_resources);
			gf_free(ires->data);
			gf_free(ires);
		}
		gf_list_del(ctx->ttml_resources);
	}
	if (ctx->div_nodes_list)
		gf_list_del(ctx->div_nodes_list);

	if (ctx->file_name) gf_free(ctx->file_name);
}