static void gf_webvtt_flush_sample(void *user, GF_WebVTTSample *samp)
{
	u64 start, end;
	GF_TXTIn *ctx = (GF_TXTIn *)user;
	GF_ISOSample *s;

	start = gf_webvtt_sample_get_start(samp);
	end = gf_webvtt_sample_get_end(samp);

	if (ctx->seek_state==2) {
		Double tsend = (Double) end;
		tsend /= 1000;
		if (tsend<ctx->start_range) return;
		ctx->seek_state = 0;
	}

	s = gf_isom_webvtt_to_sample(samp);
	if (s) {
		GF_FilterPacket *pck;
		u8 *pck_data;

		pck = gf_filter_pck_new_alloc(ctx->opid, s->dataLength, &pck_data);
		if (pck) {
			memcpy(pck_data, s->data, s->dataLength);
			gf_filter_pck_set_cts(pck, (u64) (ctx->timescale * start / 1000) );
			gf_filter_pck_set_sap(pck, GF_FILTER_SAP_1);


			if (end && (end>=start) ) {
				gf_filter_pck_set_duration(pck, (u32) (ctx->timescale * (end-start) / 1000) );
			}
			gf_filter_pck_send(pck);
		}

		gf_isom_sample_del(&s);
	}
	gf_webvtt_sample_del(samp);

	gf_filter_pid_set_info(ctx->opid, GF_PROP_PID_DOWN_BYTES, &PROP_LONGUINT( gf_ftell(ctx->src )) );

	if (gf_filter_pid_would_block(ctx->opid))
		gf_webvtt_parser_suspend(ctx->vttparser);

}