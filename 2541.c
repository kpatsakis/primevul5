static GF_Err swf_svg_add_iso_header(void *user, const u8 *data, u32 length, Bool isHeader)
{
	GF_TXTIn *ctx = (GF_TXTIn *)user;

	if (isHeader) {
		if (!ctx->hdr_parsed) {
			gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_DECODER_CONFIG, &PROP_DATA((char *)data, (u32) ( strlen(data)+1 ) )  );
			ctx->hdr_parsed = GF_TRUE;
		}
	} else if (!ctx->seek_state) {
		GF_FilterPacket *pck;
		u8 *pck_data;
		pck = gf_filter_pck_new_alloc(ctx->opid, length, &pck_data);
		if (pck) {
			memcpy(pck_data, data, length);
			gf_filter_pck_set_framing(pck, GF_FALSE, GF_TRUE);

			gf_filter_pck_send(pck);
		}
	}
	return GF_OK;
}