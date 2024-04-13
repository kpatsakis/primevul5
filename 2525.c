static void ttml_reset_intervals(GF_TXTIn *ctx)
{
	while (gf_list_count(ctx->intervals)) {
		TTMLInterval *ival = gf_list_pop_back(ctx->intervals);
		if (ival->resources) {
			while (gf_list_count(ival->resources)) {
				TTMLRes *ires = gf_list_pop_back(ival->resources);
				if (!ires->global) {
					gf_free(ires->data);
					gf_free(ires);
				}
			}
			gf_list_del(ival->resources);
		}
		gf_free(ival);
	}
}