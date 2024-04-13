static GF_Err txtin_process_ttxt(GF_Filter *filter, GF_TXTIn *ctx)
{
	u32 j, k;
	GF_XMLNode *root, *ext;

	if (!ctx->is_setup) {
		ctx->is_setup = GF_TRUE;
		return txtin_setup_ttxt(filter, ctx);
	}
	if (!ctx->opid) return GF_NON_COMPLIANT_BITSTREAM;
	if (!ctx->playstate) return GF_OK;
	else if (ctx->playstate==2) return GF_EOS;

	if (ctx->seek_state==1) {
		ctx->seek_state = 2;
		ctx->cur_child_idx = 0;
	}
	root = gf_xml_dom_get_root(ctx->parser);

	for (; ctx->cur_child_idx < ctx->nb_children; ctx->cur_child_idx++) {
		GF_TextSample * samp;
		u32 ts, descIndex;
		Bool has_text = GF_FALSE;
		GF_XMLAttribute *att;
		GF_XMLNode *node = (GF_XMLNode*) gf_list_get(root->content, ctx->cur_child_idx);

		if (node->type) {
			continue;
		}
		/*sample text*/
		else if (strcmp(node->name, "TextSample")) continue;

		samp = gf_isom_new_text_sample();
		ts = 0;
		descIndex = 1;
		ctx->last_sample_empty = GF_TRUE;

		j=0;
		while ( (att=(GF_XMLAttribute*)gf_list_enum(node->attributes, &j))) {
			if (!strcmp(att->name, "sampleTime")) {
				u32 h, m, s, ms;
				if (sscanf(att->value, "%u:%u:%u.%u", &h, &m, &s, &ms) == 4) {
					ts = (h*3600 + m*60 + s)*1000 + ms;
				} else {
					ts = (u32) (atof(att->value) * 1000);
				}
			}
			else if (!strcmp(att->name, "sampleDescriptionIndex")) descIndex = atoi(att->value);
			else if (!strcmp(att->name, "text")) {
				u32 len;
				char *str = ttxt_parse_string(att->value, GF_TRUE);
				len = (u32) strlen(str);
				gf_isom_text_add_text(samp, str, len);
				ctx->last_sample_empty = len ? GF_FALSE : GF_TRUE;
				has_text = GF_TRUE;
			}
			else if (!strcmp(att->name, "scrollDelay")) gf_isom_text_set_scroll_delay(samp, (u32) (1000*atoi(att->value)));
			else if (!strcmp(att->name, "highlightColor")) gf_isom_text_set_highlight_color(samp, ttxt_get_color(att->value));
			else if (!strcmp(att->name, "wrap") && !strcmp(att->value, "Automatic")) gf_isom_text_set_wrap(samp, 0x01);
		}

		/*get all modifiers*/
		j=0;
		while ( (ext=(GF_XMLNode*)gf_list_enum(node->content, &j))) {
			if (!has_text && (ext->type==GF_XML_TEXT_TYPE)) {
				u32 len;
				char *str = ttxt_parse_string(ext->name, GF_FALSE);
				len = (u32) strlen(str);
				gf_isom_text_add_text(samp, str, len);
				ctx->last_sample_empty = len ? GF_FALSE : GF_TRUE;
				has_text = GF_TRUE;
			}
			if (ext->type) continue;

			if (!stricmp(ext->name, "Style")) {
				GF_StyleRecord r;
				ttxt_parse_text_style(ctx, ext, &r);
				gf_isom_text_add_style(samp, &r);
			}
			else if (!stricmp(ext->name, "TextBox")) {
				GF_BoxRecord r;
				ttxt_parse_text_box(ext, &r);
				gf_isom_text_set_box(samp, r.top, r.left, r.bottom, r.right);
			}
			else if (!stricmp(ext->name, "Highlight")) {
				u16 start, end;
				start = end = 0;
				k=0;
				while ( (att=(GF_XMLAttribute *)gf_list_enum(ext->attributes, &k))) {
					if (!strcmp(att->name, "fromChar")) start = atoi(att->value);
					else if (!strcmp(att->name, "toChar")) end = atoi(att->value);
				}
				gf_isom_text_add_highlight(samp, start, end);
			}
			else if (!stricmp(ext->name, "Blinking")) {
				u16 start, end;
				start = end = 0;
				k=0;
				while ( (att=(GF_XMLAttribute *)gf_list_enum(ext->attributes, &k))) {
					if (!strcmp(att->name, "fromChar")) start = atoi(att->value);
					else if (!strcmp(att->name, "toChar")) end = atoi(att->value);
				}
				gf_isom_text_add_blink(samp, start, end);
			}
			else if (!stricmp(ext->name, "HyperLink")) {
				u16 start, end;
				char *url, *url_tt;
				start = end = 0;
				url = url_tt = NULL;
				k=0;
				while ( (att=(GF_XMLAttribute *)gf_list_enum(ext->attributes, &k))) {
					if (!strcmp(att->name, "fromChar")) start = atoi(att->value);
					else if (!strcmp(att->name, "toChar")) end = atoi(att->value);
					else if (!strcmp(att->name, "URL")) url = gf_strdup(att->value);
					else if (!strcmp(att->name, "URLToolTip")) url_tt = gf_strdup(att->value);
				}
				gf_isom_text_add_hyperlink(samp, url, url_tt, start, end);
				if (url) gf_free(url);
				if (url_tt) gf_free(url_tt);
			}
			else if (!stricmp(ext->name, "Karaoke")) {
				u32 startTime;
				GF_XMLNode *krok;
				startTime = 0;
				k=0;
				while ( (att=(GF_XMLAttribute *)gf_list_enum(ext->attributes, &k))) {
					if (!strcmp(att->name, "startTime")) startTime = (u32) (1000*atof(att->value));
				}
				gf_isom_text_add_karaoke(samp, startTime);
				k=0;
				while ( (krok=(GF_XMLNode*)gf_list_enum(ext->content, &k))) {
					u16 start, end;
					u32 endTime, m;
					if (krok->type) continue;
					if (strcmp(krok->name, "KaraokeRange")) continue;
					start = end = 0;
					endTime = 0;
					m=0;
					while ( (att=(GF_XMLAttribute *)gf_list_enum(krok->attributes, &m))) {
						if (!strcmp(att->name, "fromChar")) start = atoi(att->value);
						else if (!strcmp(att->name, "toChar")) end = atoi(att->value);
						else if (!strcmp(att->name, "endTime")) endTime = (u32) (1000*atof(att->value));
					}
					gf_isom_text_set_karaoke_segment(samp, endTime, start, end);
				}
			}
		}

		if (!descIndex) descIndex = 1;
		if (descIndex != ctx->last_desc_idx) {
			GF_PropertyValue *dcd;
			ctx->last_desc_idx = descIndex;
			dcd = gf_list_get(ctx->text_descs, descIndex-1);
			gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_DECODER_CONFIG, dcd);
		}

		/*in MP4 we must start at T=0, so add an empty sample*/
		if (ts && ctx->first_samp) {
			GF_TextSample * firstsamp = gf_isom_new_text_sample();
			txtin_process_send_text_sample(ctx, firstsamp, 0, 0, GF_TRUE);
			gf_isom_delete_text_sample(firstsamp);
		}
		ctx->first_samp = GF_FALSE;

		txtin_process_send_text_sample(ctx, samp, ts, 0, GF_TRUE);

		gf_isom_delete_text_sample(samp);

		if (ctx->last_sample_empty) {
			ctx->last_sample_duration = ts - ctx->last_sample_duration;
		} else {
			ctx->last_sample_duration = ts;
		}

		if (gf_filter_pid_would_block(ctx->opid)) {
			ctx->cur_child_idx++;
			return GF_OK;
		}
	}

	if (ctx->last_sample_empty) {
		//this is a bit ugly, in regular streaming mode we don't want to remove empty samples
		//howvere the last one can be removed, adjusting the duration of the previous one.
		//doing this here is problematic if the loader is sent a new ttxt file, we would have a cue termination sample
		//we therefore share that info through pid, and let the final user (muxer& co) decide what to do
		gf_filter_pid_set_info_str( ctx->opid, "ttxt:rem_last", &PROP_BOOL(GF_TRUE) );
		gf_filter_pid_set_info_str( ctx->opid, "ttxt:last_dur", &PROP_UINT((u32) ctx->last_sample_duration) );
	}

	return GF_EOS;
}