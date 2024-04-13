static GF_Err txtin_setup_ttxt(GF_Filter *filter, GF_TXTIn *ctx)
{
	GF_Err e;
	u32 j, k, ID, OCR_ES_ID;
	u64 file_size;
	GF_XMLNode *root, *ext;
	GF_PropertyValue *dcd;

	ctx->parser = gf_xml_dom_new();
	e = gf_xml_dom_parse(ctx->parser, ctx->file_name, ttxt_dom_progress, ctx);
	if (e) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[TXTIn] Error parsing TTXT file: Line %d - %s\n", gf_xml_dom_get_line(ctx->parser), gf_xml_dom_get_error(ctx->parser)));
		return e;
	}
	root = gf_xml_dom_get_root(ctx->parser);

	if (strcmp(root->name, "TextStream")) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[TXTIn] Invalid Timed Text file - expecting \"TextStream\" got %s", root->name));
		return GF_NON_COMPLIANT_BITSTREAM;
	}
	file_size = ctx->end;
	ctx->end = 0;

	/*setup track in 3GP format directly (no ES desc)*/
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

	ctx->nb_children = gf_list_count(root->content);

	ctx->cur_child_idx = 0;
	for (ctx->cur_child_idx=0; ctx->cur_child_idx < ctx->nb_children; ctx->cur_child_idx++) {
		GF_XMLNode *node = (GF_XMLNode*) gf_list_get(root->content, ctx->cur_child_idx);

		if (node->type) {
			continue;
		}

		if (!strcmp(node->name, "TextStreamHeader")) {
			GF_XMLNode *sdesc;
			s32 w, h, tx, ty, layer;
			u32 tref_id;
			GF_XMLAttribute *att;
			w = ctx->width;
			h = ctx->height;
			tx = ctx->txtx;
			ty = ctx->txty;
			layer = ctx->zorder;
			tref_id = 0;

			j=0;
			while ( (att=(GF_XMLAttribute *)gf_list_enum(node->attributes, &j))) {
				if (!strcmp(att->name, "width")) w = atoi(att->value);
				else if (!strcmp(att->name, "height")) h = atoi(att->value);
				else if (!strcmp(att->name, "layer")) layer = atoi(att->value);
				else if (!strcmp(att->name, "translation_x")) tx = atoi(att->value);
				else if (!strcmp(att->name, "translation_y")) ty = atoi(att->value);
				else if (!strcmp(att->name, "trefID")) tref_id = atoi(att->value);
			}

			if (tref_id) {
				gf_filter_pid_set_property_str(ctx->opid, "tref:chap", &PROP_UINT(tref_id) );
			}

			if (w) gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_WIDTH, &PROP_UINT(w) );
			if (h) gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_HEIGHT, &PROP_UINT(h) );
			if (tx) gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_TRANS_X, &PROP_UINT(tx) );
			if (ty) gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_TRANS_X, &PROP_UINT(ty) );
			if (layer) gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_ZORDER, &PROP_SINT(ctx->zorder) );
			if (ctx->lang) gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_LANGUAGE, &PROP_STRING( ctx->lang) );

			j=0;
			while ( (sdesc=(GF_XMLNode*)gf_list_enum(node->content, &j))) {
				if (sdesc->type) continue;

				if (!strcmp(sdesc->name, "TextSampleDescription")) {
					GF_TextSampleDescriptor td;
					memset(&td, 0, sizeof(GF_TextSampleDescriptor));
					td.tag = GF_ODF_TEXT_CFG_TAG;
					td.vert_justif = (s8) -1;
					td.default_style.fontID = 1;
					td.default_style.font_size = ctx->fontsize;

					k=0;
					while ( (att=(GF_XMLAttribute *)gf_list_enum(sdesc->attributes, &k))) {
						if (!strcmp(att->name, "horizontalJustification")) {
							if (!stricmp(att->value, "center")) td.horiz_justif = 1;
							else if (!stricmp(att->value, "right")) td.horiz_justif = (s8) -1;
							else if (!stricmp(att->value, "left")) td.horiz_justif = 0;
						}
						else if (!strcmp(att->name, "verticalJustification")) {
							if (!stricmp(att->value, "center")) td.vert_justif = 1;
							else if (!stricmp(att->value, "bottom")) td.vert_justif = (s8) -1;
							else if (!stricmp(att->value, "top")) td.vert_justif = 0;
						}
						else if (!strcmp(att->name, "backColor")) td.back_color = ttxt_get_color(att->value);
						else if (!strcmp(att->name, "verticalText") && !stricmp(att->value, "yes") ) td.displayFlags |= GF_TXT_VERTICAL;
						else if (!strcmp(att->name, "fillTextRegion") && !stricmp(att->value, "yes") ) td.displayFlags |= GF_TXT_FILL_REGION;
						else if (!strcmp(att->name, "continuousKaraoke") && !stricmp(att->value, "yes") ) td.displayFlags |= GF_TXT_KARAOKE;
						else if (!strcmp(att->name, "scroll")) {
							if (!stricmp(att->value, "inout")) td.displayFlags |= GF_TXT_SCROLL_IN | GF_TXT_SCROLL_OUT;
							else if (!stricmp(att->value, "in")) td.displayFlags |= GF_TXT_SCROLL_IN;
							else if (!stricmp(att->value, "out")) td.displayFlags |= GF_TXT_SCROLL_OUT;
						}
						else if (!strcmp(att->name, "scrollMode")) {
							u32 scroll_mode = GF_TXT_SCROLL_CREDITS;
							if (!stricmp(att->value, "Credits")) scroll_mode = GF_TXT_SCROLL_CREDITS;
							else if (!stricmp(att->value, "Marquee")) scroll_mode = GF_TXT_SCROLL_MARQUEE;
							else if (!stricmp(att->value, "Right")) scroll_mode = GF_TXT_SCROLL_RIGHT;
							else if (!stricmp(att->value, "Down")) scroll_mode = GF_TXT_SCROLL_DOWN;
							td.displayFlags |= ((scroll_mode<<7) & GF_TXT_SCROLL_DIRECTION);
						}
					}

					k=0;
					while ( (ext=(GF_XMLNode*)gf_list_enum(sdesc->content, &k))) {
						if (ext->type) continue;
						if (!strcmp(ext->name, "TextBox")) ttxt_parse_text_box(ext, &td.default_pos);
						else if (!strcmp(ext->name, "Style")) ttxt_parse_text_style(ctx, ext, &td.default_style);
						else if (!strcmp(ext->name, "FontTable")) {
							GF_XMLNode *ftable;
							u32 z=0;
							while ( (ftable=(GF_XMLNode*)gf_list_enum(ext->content, &z))) {
								u32 m;
								if (ftable->type || strcmp(ftable->name, "FontTableEntry")) continue;
								td.font_count += 1;
								td.fonts = (GF_FontRecord*)gf_realloc(td.fonts, sizeof(GF_FontRecord)*td.font_count);
								m=0;
								while ( (att=(GF_XMLAttribute *)gf_list_enum(ftable->attributes, &m))) {
									if (!stricmp(att->name, "fontID")) td.fonts[td.font_count-1].fontID = atoi(att->value);
									else if (!stricmp(att->name, "fontName")) td.fonts[td.font_count-1].fontName = gf_strdup(att->value);
								}
							}
						}
					}
					if (ctx->nodefbox) {
						td.default_pos.top = td.default_pos.left = td.default_pos.right = td.default_pos.bottom = 0;
					} else {
						if ((td.default_pos.bottom==td.default_pos.top) || (td.default_pos.right==td.default_pos.left)) {
							td.default_pos.top = td.default_pos.left = 0;
							td.default_pos.right = w;
							td.default_pos.bottom = h;
						}
					}
					if (!td.fonts) {
						td.font_count = 1;
						td.fonts = (GF_FontRecord*)gf_malloc(sizeof(GF_FontRecord));
						td.fonts[0].fontID = 1;
						td.fonts[0].fontName = gf_strdup("Serif");
					}
					GF_SAFEALLOC(dcd, GF_PropertyValue);
					if (dcd) {
						dcd->type = GF_PROP_DATA;

						gf_odf_tx3g_write(&td, &dcd->value.data.ptr, &dcd->value.data.size);
						if (!ctx->text_descs) ctx->text_descs = gf_list_new();
						gf_list_add(ctx->text_descs, dcd);
					}

					for (k=0; k<td.font_count; k++) gf_free(td.fonts[k].fontName);
					gf_free(td.fonts);
				}
			}
		}
		else {
			break;
		}
	}

	if (!ctx->text_descs) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[TXTIn] Invalid Timed Text file - text stream header not found or empty\n"));
		return GF_NON_COMPLIANT_BITSTREAM;
	}
	dcd = gf_list_get(ctx->text_descs, 0);
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_DECODER_CONFIG, dcd);
	ctx->last_desc_idx = 1;

	ctx->first_samp = GF_TRUE;
	ctx->last_sample_empty = GF_FALSE;
	ctx->last_sample_duration = 0;

	txtin_probe_duration(ctx);

	return GF_OK;
}