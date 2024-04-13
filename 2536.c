static GF_Err txtin_setup_srt(GF_Filter *filter, GF_TXTIn *ctx)
{
	u32 ID, OCR_ES_ID, dsi_len, file_size;
	u8 *dsi;
	GF_TextSampleDescriptor *sd;

	ctx->src = gf_fopen(ctx->file_name, "rb");
	if (!ctx->src) return GF_URL_ERROR;

	file_size = (u32) gf_fsize(ctx->src);

	ctx->unicode_type = gf_text_get_utf_type(ctx->src);
	if (ctx->unicode_type<0) {
		gf_fclose(ctx->src);
		ctx->src = NULL;
		GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[TXTIn] Unsupported SRT UTF encoding\n"));
		return GF_NOT_SUPPORTED;
	}

	if (!ctx->timescale) ctx->timescale = 1000;
	OCR_ES_ID = ID = 0;

	if (!ctx->opid) ctx->opid = gf_filter_pid_new(filter);
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_STREAM_TYPE, &PROP_UINT(GF_STREAM_TEXT) );
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_CODECID, &PROP_UINT(GF_CODECID_TX3G) );
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_TIMESCALE, &PROP_UINT(ctx->timescale) );
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_DOWN_SIZE, &PROP_LONGUINT(file_size) );

	if (!ID) ID = 1;
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_ID, &PROP_UINT(ID) );
	if (OCR_ES_ID) gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_CLOCK_ID, &PROP_UINT(OCR_ES_ID) );
	if (ctx->width) gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_WIDTH, &PROP_UINT(ctx->width) );
	if (ctx->height) gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_HEIGHT, &PROP_UINT(ctx->height) );
	if (ctx->zorder) gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_ZORDER, &PROP_SINT(ctx->zorder) );
	if (ctx->lang) gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_LANGUAGE, &PROP_STRING( ctx->lang) );

	sd = (GF_TextSampleDescriptor*)gf_odf_desc_new(GF_ODF_TX3G_TAG);
	sd->fonts = (GF_FontRecord*)gf_malloc(sizeof(GF_FontRecord));
	sd->font_count = 1;
	sd->fonts[0].fontID = 1;
	sd->fonts[0].fontName = gf_strdup(ctx->fontname ? ctx->fontname : "Serif");
	sd->back_color = 0x00000000;	/*transparent*/
	sd->default_style.fontID = 1;
	sd->default_style.font_size = ctx->fontsize;
	sd->default_style.text_color = 0xFFFFFFFF;	/*white*/
	sd->default_style.style_flags = 0;
	sd->horiz_justif = 1; /*center of scene*/
	sd->vert_justif = (s8) -1;	/*bottom of scene*/

	if (ctx->nodefbox) {
		sd->default_pos.top = sd->default_pos.left = sd->default_pos.right = sd->default_pos.bottom = 0;
	} else if ((sd->default_pos.bottom==sd->default_pos.top) || (sd->default_pos.right==sd->default_pos.left)) {
		sd->default_pos.left = ctx->txtx;
		sd->default_pos.top = ctx->txty;
		sd->default_pos.right = ctx->width + sd->default_pos.left;
		sd->default_pos.bottom = ctx->height + sd->default_pos.top;
	}

	/*store attribs*/
	ctx->style = sd->default_style;
	gf_odf_tx3g_write(sd, &dsi, &dsi_len);
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_DECODER_CONFIG, &PROP_DATA_NO_COPY(dsi, dsi_len) );

	gf_odf_desc_del((GF_Descriptor *)sd);

	ctx->default_color = ctx->style.text_color;
	ctx->samp = gf_isom_new_text_sample();
	ctx->state = 0;
	ctx->end = ctx->prev_end = ctx->start = 0;
	ctx->first_samp = GF_TRUE;
	ctx->curLine = 0;

	txtin_probe_duration(ctx);
	return GF_OK;
}