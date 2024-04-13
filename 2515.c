static GF_Err txtin_process(GF_Filter *filter)
{
	GF_TXTIn *ctx = gf_filter_get_udta(filter);
	GF_FilterPacket *pck;
	GF_Err e;
	Bool start, end;
	pck = gf_filter_pid_get_packet(ctx->ipid);
	if (!pck) {
		return GF_OK;
	}
	gf_filter_pck_get_framing(pck, &start, &end);
	if (!end) {
		gf_filter_pid_drop_packet(ctx->ipid);
		return GF_OK;
	}
	//file is loaded

	e = ctx->text_process(filter, ctx);


	if (e==GF_EOS) {
		//keep input alive until end of stream, so that we keep getting called
		gf_filter_pid_drop_packet(ctx->ipid);
		if (gf_filter_pid_is_eos(ctx->ipid))
			gf_filter_pid_set_eos(ctx->opid);
	}
	return e;
}