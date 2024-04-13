static GF_Err ttml_push_res(GF_TXTIn *ctx, TTMLInterval *interval, u8 *f_data, u32 f_size)
{
	GF_Err e;
	TTMLRes *res;
	GF_List *res_list;
	if (interval) {
		if (!interval->resources) {
			if (ctx->ttml_resources)
				interval->resources = gf_list_clone(ctx->ttml_resources);
			else
				interval->resources = gf_list_new();
		}
		res_list = interval->resources;
	} else {
		if (!ctx->ttml_resources) {
			ctx->ttml_resources = gf_list_new();
		}
		res_list = ctx->ttml_resources;
	}
	if (!res_list) {
		gf_free(f_data);
		return GF_OUT_OF_MEM;
	}
	GF_SAFEALLOC(res, TTMLRes)
	if (!res) {
		gf_free(f_data);
		return GF_OUT_OF_MEM;
	}
	res->size = f_size;
	res->data = f_data;
	if (!interval)
		res->global = GF_TRUE;

	e = gf_list_add(res_list, res);
	if (e) {
		gf_free(res);
		gf_free(f_data);
		return e;
	}
	return GF_OK;
}