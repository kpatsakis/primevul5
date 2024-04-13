static void txtin_process_send_text_sample(GF_TXTIn *ctx, GF_TextSample *txt_samp, u64 ts, u32 duration, Bool is_rap)
{
	GF_FilterPacket *dst_pck;
	u8 *pck_data;
	u32 size;

	if (ctx->seek_state==2) {
		Double end = (Double) (ts+duration);
		end /= 1000;
		if (end < ctx->start_range) return;
		ctx->seek_state = 0;
	}

	size = gf_isom_text_sample_size(txt_samp);

	dst_pck = gf_filter_pck_new_alloc(ctx->opid, size, &pck_data);
	if (!dst_pck) return;

	gf_bs_reassign_buffer(ctx->bs_w, pck_data, size);
	gf_isom_text_sample_write_bs(txt_samp, ctx->bs_w);

	ts = gf_timestamp_rescale(ts, 1000, ctx->timescale);
	duration = (u32) gf_timestamp_rescale(duration, 1000, ctx->timescale);

	gf_filter_pck_set_sap(dst_pck, is_rap ? GF_FILTER_SAP_1 : GF_FILTER_SAP_NONE);
	gf_filter_pck_set_cts(dst_pck, ts);
	gf_filter_pck_set_duration(dst_pck, duration);

	gf_filter_pck_send(dst_pck);
}