static GF_Err txtin_process_texml(GF_Filter *filter, GF_TXTIn *ctx)
{
	u32 j, k;
	GF_StyleRecord styles[50];
	Marker marks[50];
	GF_XMLAttribute *att;
	GF_XMLNode *root;
	Bool probe_first_desc_only = GF_FALSE;

	if (!ctx->is_setup) {
		GF_Err e;

		ctx->is_setup = GF_TRUE;
		e = txtin_texml_setup(filter, ctx);
		if (e) return e;
		probe_first_desc_only = GF_TRUE;
	}
	if (!ctx->opid) return GF_NON_COMPLIANT_BITSTREAM;
	if (!ctx->playstate && !probe_first_desc_only) return GF_OK;
	else if (ctx->playstate==2) return GF_EOS;

	if (ctx->seek_state==1) {
		ctx->seek_state = 2;
		ctx->cur_child_idx = 0;
		ctx->start = 0;
	}

	root = gf_xml_dom_get_root(ctx->parser);

	for (; ctx->cur_child_idx < ctx->nb_children; ctx->cur_child_idx++) {
		GF_XMLNode *node, *desc;
		GF_TextSampleDescriptor td;
		GF_TextSample * samp = NULL;
		u64 duration;
		u32 nb_styles, nb_marks;
		Bool isRAP, same_style, same_box;

		if (probe_first_desc_only && ctx->text_descs && gf_list_count(ctx->text_descs))
			return GF_OK;

		memset(&td, 0, sizeof(GF_TextSampleDescriptor));
		node = (GF_XMLNode*)gf_list_get(root->content, ctx->cur_child_idx);
		if (node->type) continue;
		if (strcmp(node->name, "sample")) continue;

		isRAP = GF_TRUE;
		duration = 1000;
		j=0;
		while ((att=(GF_XMLAttribute *)gf_list_enum(node->attributes, &j))) {
			if (!strcmp(att->name, "duration")) duration = atoi(att->value);
			else if (!strcmp(att->name, "keyframe")) isRAP = (!stricmp(att->value, "true") ? GF_TRUE : GF_FALSE);
		}
		nb_styles = 0;
		nb_marks = 0;
		same_style = same_box = GF_FALSE;
		j=0;
		while ((desc=(GF_XMLNode*)gf_list_enum(node->content, &j))) {
			if (desc->type) continue;

			if (!strcmp(desc->name, "description")) {
				u8 *dsi;
				u32 dsi_len, stsd_idx;
				GF_XMLNode *sub;
				memset(&td, 0, sizeof(GF_TextSampleDescriptor));
				td.tag = GF_ODF_TEXT_CFG_TAG;
				td.vert_justif = (s8) -1;
				td.default_style.fontID = 1;
				td.default_style.font_size = ctx->fontsize;

				k=0;
				while ((att=(GF_XMLAttribute *)gf_list_enum(desc->attributes, &k))) {
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
					else if (!strcmp(att->name, "backgroundColor")) td.back_color = tx3g_get_color(att->value);
					else if (!strcmp(att->name, "displayFlags")) {
						Bool rev_scroll = GF_FALSE;
						if (strstr(att->value, "scroll")) {
							u32 scroll_mode = 0;
							if (strstr(att->value, "scrollIn")) td.displayFlags |= GF_TXT_SCROLL_IN;
							if (strstr(att->value, "scrollOut")) td.displayFlags |= GF_TXT_SCROLL_OUT;
							if (strstr(att->value, "reverse")) rev_scroll = GF_TRUE;
							if (strstr(att->value, "horizontal")) scroll_mode = rev_scroll ? GF_TXT_SCROLL_RIGHT : GF_TXT_SCROLL_MARQUEE;
							else scroll_mode = (rev_scroll ? GF_TXT_SCROLL_DOWN : GF_TXT_SCROLL_CREDITS);
							td.displayFlags |= (scroll_mode<<7) & GF_TXT_SCROLL_DIRECTION;
						}
						/*TODO FIXME: check in QT doc !!*/
						if (strstr(att->value, "writeTextVertically")) td.displayFlags |= GF_TXT_VERTICAL;
						if (!strcmp(att->name, "continuousKaraoke")) td.displayFlags |= GF_TXT_KARAOKE;
					}
				}

				k=0;
				while ((sub=(GF_XMLNode*)gf_list_enum(desc->content, &k))) {
					if (sub->type) continue;
					if (!strcmp(sub->name, "defaultTextBox")) tx3g_parse_text_box(sub, &td.default_pos);
					else if (!strcmp(sub->name, "fontTable")) {
						GF_XMLNode *ftable;
						u32 m=0;
						while ((ftable=(GF_XMLNode*)gf_list_enum(sub->content, &m))) {
							if (ftable->type) continue;
							if (!strcmp(ftable->name, "font")) {
								u32 n=0;
								td.font_count += 1;
								td.fonts = (GF_FontRecord*)gf_realloc(td.fonts, sizeof(GF_FontRecord)*td.font_count);
								while ((att=(GF_XMLAttribute *)gf_list_enum(ftable->attributes, &n))) {
									if (!stricmp(att->name, "id")) td.fonts[td.font_count-1].fontID = atoi(att->value);
									else if (!stricmp(att->name, "name")) td.fonts[td.font_count-1].fontName = gf_strdup(att->value);
								}
							}
						}
					}
					else if (!strcmp(sub->name, "sharedStyles")) {
						GF_XMLNode *style, *ftable;
						u32 m=0;
						while ((style=(GF_XMLNode*)gf_list_enum(sub->content, &m))) {
							if (style->type) continue;
							if (!strcmp(style->name, "style")) break;
						}
						if (style) {
							char *cur;
							s32 start=0;
							char css_style[1024], css_val[1024];
							memset(&styles[nb_styles], 0, sizeof(GF_StyleRecord));
							m=0;
							while ( (att=(GF_XMLAttribute *)gf_list_enum(style->attributes, &m))) {
								if (!strcmp(att->name, "id")) styles[nb_styles].startCharOffset = atoi(att->value);
							}
							m=0;
							while ( (ftable=(GF_XMLNode*)gf_list_enum(style->content, &m))) {
								if (ftable->type) break;
							}
							cur = ftable ? ftable->name : NULL;
							while (cur) {
								start = gf_token_get_strip(cur, 0, "{:", " ", css_style, 1024);
								if (start <0) break;
								start = gf_token_get_strip(cur, start, ":}", " ", css_val, 1024);
								if (start <0) break;
								cur = strchr(cur+start, '{');

								if (!strcmp(css_style, "font-table")) {
									u32 z;
									styles[nb_styles].fontID = atoi(css_val);
									for (z=0; z<td.font_count; z++) {
										if (td.fonts[z].fontID == styles[nb_styles].fontID)
											break;
									}
								}
								else if (!strcmp(css_style, "font-size")) styles[nb_styles].font_size = atoi(css_val);
								else if (!strcmp(css_style, "font-style") && !strcmp(css_val, "italic")) styles[nb_styles].style_flags |= GF_TXT_STYLE_ITALIC;
								else if (!strcmp(css_style, "font-weight") && !strcmp(css_val, "bold")) styles[nb_styles].style_flags |= GF_TXT_STYLE_BOLD;
								else if (!strcmp(css_style, "text-decoration") && !strcmp(css_val, "underline")) styles[nb_styles].style_flags |= GF_TXT_STYLE_UNDERLINED;
								else if (!strcmp(css_style, "text-decoration") && !strcmp(css_val, "strikethrough")) styles[nb_styles].style_flags |= GF_TXT_STYLE_STRIKETHROUGH;
								else if (!strcmp(css_style, "color")) styles[nb_styles].text_color = tx3g_get_color(css_val);
							}
							if (!nb_styles) td.default_style = styles[0];
							nb_styles++;
						}
					}

				}
				if ((td.default_pos.bottom==td.default_pos.top) || (td.default_pos.right==td.default_pos.left)) {
					td.default_pos.top = ctx->txty;
					td.default_pos.left = ctx->txtx;
					td.default_pos.right = ctx->width;
					td.default_pos.bottom = ctx->height;
				}
				if (!td.fonts) {
					td.font_count = 1;
					td.fonts = (GF_FontRecord*)gf_malloc(sizeof(GF_FontRecord));
					td.fonts[0].fontID = 1;
					td.fonts[0].fontName = gf_strdup( ctx->fontname ? ctx->fontname : "Serif");
				}

				gf_odf_tx3g_write(&td, &dsi, &dsi_len);
				stsd_idx = 0;
				for (k=0; ctx->text_descs && k<gf_list_count(ctx->text_descs); k++) {
					GF_PropertyValue *d = gf_list_get(ctx->text_descs, k);
					if (d->value.data.size != dsi_len) continue;
					if (! memcmp(d->value.data.ptr, dsi, dsi_len)) {
						stsd_idx = k+1;
						break;
					}
				}
				if (stsd_idx) {
					gf_free(dsi);
				} else {
					GF_PropertyValue *d;
					GF_SAFEALLOC(d, GF_PropertyValue);
					if (!d) return GF_OUT_OF_MEM;
					d->type = GF_PROP_DATA;
					d->value.data.ptr = dsi;
					d->value.data.size = dsi_len;
					if (!ctx->text_descs) ctx->text_descs = gf_list_new();
					gf_list_add(ctx->text_descs, d);
					stsd_idx = gf_list_count(ctx->text_descs);
				}
				if (stsd_idx != ctx->last_desc_idx) {
					ctx->last_desc_idx = stsd_idx;
					GF_PropertyValue *d = gf_list_get(ctx->text_descs, stsd_idx-1);
					gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_DECODER_CONFIG, d);
				}

				for (k=0; k<td.font_count; k++) gf_free(td.fonts[k].fontName);
				gf_free(td.fonts);

				if (probe_first_desc_only)
					return GF_OK;
			}
			else if (!strcmp(desc->name, "sampleData")) {
				GF_XMLNode *sub;
				u16 start, end;
				u32 styleID;
				u32 nb_chars, txt_len, m;
				nb_chars = 0;

				samp = gf_isom_new_text_sample();

				k=0;
				while ((att=(GF_XMLAttribute *)gf_list_enum(desc->attributes, &k))) {
					if (!strcmp(att->name, "targetEncoding") && !strcmp(att->value, "utf16")) ;//is_utf16 = 1;
					else if (!strcmp(att->name, "scrollDelay")) gf_isom_text_set_scroll_delay(samp, atoi(att->value) );
					else if (!strcmp(att->name, "highlightColor")) gf_isom_text_set_highlight_color(samp, tx3g_get_color(att->value));
				}
				start = end = 0;
				k=0;
				while ((sub=(GF_XMLNode*)gf_list_enum(desc->content, &k))) {
					if (sub->type) continue;
					if (!strcmp(sub->name, "text")) {
						GF_XMLNode *text;
						styleID = 0;
						m=0;
						while ((att=(GF_XMLAttribute *)gf_list_enum(sub->attributes, &m))) {
							if (!strcmp(att->name, "styleID")) styleID = atoi(att->value);
						}
						txt_len = 0;

						m=0;
						while ((text=(GF_XMLNode*)gf_list_enum(sub->content, &m))) {
							if (!text->type) {
								if (!strcmp(text->name, "marker")) {
									u32 z;
									memset(&marks[nb_marks], 0, sizeof(Marker));
									marks[nb_marks].pos = nb_chars+txt_len;

									z = 0;
									while ( (att=(GF_XMLAttribute *)gf_list_enum(text->attributes, &z))) {
										if (!strcmp(att->name, "id")) marks[nb_marks].id = atoi(att->value);
									}
									nb_marks++;
								}
							} else if (text->type==GF_XML_TEXT_TYPE) {
								txt_len += (u32) strlen(text->name);
								gf_isom_text_add_text(samp, text->name, (u32) strlen(text->name));
							}
						}
						if (styleID && (!same_style || (td.default_style.startCharOffset != styleID))) {
							GF_StyleRecord st = td.default_style;
							for (m=0; m<nb_styles; m++) {
								if (styles[m].startCharOffset==styleID) {
									st = styles[m];
									break;
								}
							}
							st.startCharOffset = nb_chars;
							st.endCharOffset = nb_chars + txt_len;
							gf_isom_text_add_style(samp, &st);
						}
						nb_chars += txt_len;
					}
					else if (!stricmp(sub->name, "highlight")) {
						m=0;
						while ((att=(GF_XMLAttribute *)gf_list_enum(sub->attributes, &m))) {
							if (!strcmp(att->name, "startMarker")) GET_MARKER_POS(start, 0)
								else if (!strcmp(att->name, "endMarker")) GET_MARKER_POS(end, 1)
								}
						gf_isom_text_add_highlight(samp, start, end);
					}
					else if (!stricmp(sub->name, "blink")) {
						m=0;
						while ((att=(GF_XMLAttribute *)gf_list_enum(sub->attributes, &m))) {
							if (!strcmp(att->name, "startMarker")) GET_MARKER_POS(start, 0)
								else if (!strcmp(att->name, "endMarker")) GET_MARKER_POS(end, 1)
								}
						gf_isom_text_add_blink(samp, start, end);
					}
					else if (!stricmp(sub->name, "link")) {
						char *url, *url_tt;
						url = url_tt = NULL;
						m=0;
						while ((att=(GF_XMLAttribute *)gf_list_enum(sub->attributes, &m))) {
							if (!strcmp(att->name, "startMarker")) GET_MARKER_POS(start, 0)
								else if (!strcmp(att->name, "endMarker")) GET_MARKER_POS(end, 1)
									else if (!strcmp(att->name, "URL") || !strcmp(att->name, "href")) url = gf_strdup(att->value);
									else if (!strcmp(att->name, "URLToolTip") || !strcmp(att->name, "altString")) url_tt = gf_strdup(att->value);
						}
						gf_isom_text_add_hyperlink(samp, url, url_tt, start, end);
						if (url) gf_free(url);
						if (url_tt) gf_free(url_tt);
					}
					else if (!stricmp(sub->name, "karaoke")) {
						u32 time = 0;
						GF_XMLNode *krok;
						m=0;
						while ((att=(GF_XMLAttribute *)gf_list_enum(sub->attributes, &m))) {
							if (!strcmp(att->name, "startTime")) time = atoi(att->value);
						}
						gf_isom_text_add_karaoke(samp, time);
						m=0;
						while ((krok=(GF_XMLNode*)gf_list_enum(sub->content, &m))) {
							u32 u=0;
							if (krok->type) continue;
							if (strcmp(krok->name, "run")) continue;
							start = end = 0;
							while ((att=(GF_XMLAttribute *)gf_list_enum(krok->attributes, &u))) {
								if (!strcmp(att->name, "startMarker")) GET_MARKER_POS(start, 0)
									else if (!strcmp(att->name, "endMarker")) GET_MARKER_POS(end, 1)
										else if (!strcmp(att->name, "duration")) time += atoi(att->value);
							}
							gf_isom_text_set_karaoke_segment(samp, time, start, end);
						}
					}
				}
			}
		}
		/*OK, let's add the sample*/
		if (samp) {
			if (!same_box) gf_isom_text_set_box(samp, td.default_pos.top, td.default_pos.left, td.default_pos.bottom, td.default_pos.right);
//			if (!same_style) gf_isom_text_add_style(samp, &td.default_style);

			txtin_process_send_text_sample(ctx, samp, (ctx->start*ctx->timescale)/ctx->txml_timescale, (u32) (duration*ctx->timescale)/ctx->txml_timescale, isRAP);
			ctx->start += duration;
			gf_isom_delete_text_sample(samp);

		}
		if (gf_filter_pid_would_block(ctx->opid)) {
			ctx->cur_child_idx++;
			return GF_OK;
		}
	}

	return GF_EOS;
}