static GF_Err txtin_process_srt(GF_Filter *filter, GF_TXTIn *ctx)
{
	u32 i;
	u32 sh, sm, ss, sms, eh, em, es, ems, txt_line, char_len, char_line, j, rem_styles;
	Bool set_start_char, set_end_char, rem_color;
	u32 line, len;
	char szLine[2048], szText[2048], *ptr;
	unsigned short uniLine[5000], uniText[5000], *sptr;

	if (!ctx->is_setup) {
		ctx->is_setup = GF_TRUE;
		return txtin_setup_srt(filter, ctx);
	}
	if (!ctx->opid) return GF_NOT_SUPPORTED;
	if (!ctx->playstate) return GF_OK;
	else if (ctx->playstate==2) return GF_EOS;

	txt_line = 0;
	set_start_char = set_end_char = GF_FALSE;
	char_len = 0;

	if (ctx->seek_state == 1) {
		ctx->seek_state = 2;
		gf_fseek(ctx->src, 0, SEEK_SET);
	}

	while (1) {
		Bool is_empty = GF_FALSE;
		char *sOK = gf_text_get_utf8_line(szLine, 2048, ctx->src, ctx->unicode_type);

		if (sOK) {
			REM_TRAIL_MARKS(szLine, "\r\n\t ")

			if (ctx->unicode_type<=1) is_empty = strlen(szLine) ? GF_FALSE : GF_TRUE;
			else is_empty =  (!szLine[0] && !szLine[1]) ? GF_TRUE : GF_FALSE;
		}

		if (!sOK || is_empty) {
			u32 utf_inc = (ctx->unicode_type<=1) ? 1 : 2;
			u32 nb_empty = utf_inc;
			u32 pos = (u32) gf_ftell(ctx->src);
			if (ctx->state) {
				while (!gf_feof(ctx->src)) {
					sOK = gf_text_get_utf8_line(szLine+nb_empty, 2048-nb_empty, ctx->src, ctx->unicode_type);
					if (sOK) REM_TRAIL_MARKS((szLine+nb_empty), "\r\n\t ")

					if (!sOK) {
						gf_fseek(ctx->src, pos, SEEK_SET);
						break;
					} else if (!strlen(szLine+nb_empty)) {
						nb_empty+=utf_inc;
						continue;
					} else if (	sscanf(szLine+nb_empty, "%u", &line) == 1) {
						gf_fseek(ctx->src, pos, SEEK_SET);
						break;
					} else {
						u32 k;
						for (k=0; k<nb_empty; k++) szLine[k] = '\n';
						goto force_line;
					}
				}
			}
			ctx->style.style_flags = 0;
			ctx->style.text_color = 0xFFFFFFFF;
			ctx->style.startCharOffset = ctx->style.endCharOffset = 0;
			if (txt_line) {
				if (ctx->prev_end && (ctx->start != ctx->prev_end) && (ctx->state<=2)) {
					GF_TextSample * empty_samp = gf_isom_new_text_sample();
					txtin_process_send_text_sample(ctx, empty_samp, ctx->prev_end, (u32) (ctx->start - ctx->prev_end), GF_TRUE );
					gf_isom_delete_text_sample(empty_samp);
				}

				if (ctx->state<=2) {
					txtin_process_send_text_sample(ctx, ctx->samp,  ctx->start, (u32) (ctx->end -  ctx->start), GF_TRUE);
					ctx->prev_end = ctx->end;
				}
				txt_line = 0;
				char_len = 0;
				set_start_char = set_end_char = GF_FALSE;
				ctx->style.startCharOffset = ctx->style.endCharOffset = 0;
				gf_isom_text_reset(ctx->samp);

				gf_filter_pid_set_info(ctx->opid, GF_PROP_PID_DOWN_BYTES, &PROP_LONGUINT( gf_ftell(ctx->src )) );
			}
			ctx->state = 0;
			if (!sOK) break;
			continue;
		}

force_line:
		switch (ctx->state) {
		case 0:
			if (sscanf(szLine, "%u", &line) != 1) {
				GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[TXTIn] Bad SRT formatting - expecting number got \"%s\"\n", szLine));
				break;
			}
			if (line != ctx->curLine + 1) {
				GF_LOG(GF_LOG_WARNING, GF_LOG_PARSER, ("[TXTIn] Corrupted SRT frame %d after frame %d\n", line, ctx->curLine));
			}
			ctx->curLine = line;
			ctx->state = 1;
			break;
		case 1:
			if (sscanf(szLine, "%u:%u:%u,%u --> %u:%u:%u,%u", &sh, &sm, &ss, &sms, &eh, &em, &es, &ems) != 8) {
				if (sscanf(szLine, "%u:%u:%u.%u --> %u:%u:%u.%u", &sh, &sm, &ss, &sms, &eh, &em, &es, &ems) != 8) {
					sh = eh = 0;
					if (sscanf(szLine, "%u:%u,%u --> %u:%u,%u", &sm, &ss, &sms, &em, &es, &ems) != 6) {
						if (sscanf(szLine, "%u:%u.%u --> %u:%u.%u", &sm, &ss, &sms, &em, &es, &ems) != 6) {
							GF_LOG(GF_LOG_WARNING, GF_LOG_PARSER, ("[TXTIn] Error scanning SRT frame %d timing\n", ctx->curLine));
				    		ctx->state = 0;
							break;
						}
					}
				}
			}
			ctx->start = (3600*sh + 60*sm + ss)*1000 + sms;
			if (ctx->start < ctx->end) {
				GF_LOG(GF_LOG_WARNING, GF_LOG_PARSER, ("[TXTIn] Overlapping SRT frame %d - starts "LLD" ms is before end of previous one "LLD" ms - adjusting time stamps\n", ctx->curLine, ctx->start, ctx->end));
				ctx->start = ctx->end;
			}

			ctx->end = (3600*eh + 60*em + es)*1000 + ems;
			/*make stream start at 0 by inserting a fake AU*/
			if (ctx->first_samp && (ctx->start > 0)) {
				txtin_process_send_text_sample(ctx, ctx->samp, 0, (u32) ctx->start, GF_TRUE);
			}
			ctx->style.style_flags = 0;
			ctx->state = 2;
			if (ctx->end <= ctx->prev_end) {
				GF_LOG(GF_LOG_WARNING, GF_LOG_PARSER, ("[TXTIn] Overlapping SRT frame %d end "LLD" is at or before previous end "LLD" - removing\n", ctx->curLine, ctx->end, ctx->prev_end));
				ctx->start = ctx->end;
				ctx->state = 3;
			}
			break;

		default:
			/*reset only when text is present*/
			ctx->first_samp = GF_FALSE;

			/*go to line*/
			if (txt_line) {
				gf_isom_text_add_text(ctx->samp, "\n", 1);
				char_len += 1;
			}

			ptr = (char *) szLine;
			{
				size_t _len = gf_utf8_mbstowcs(uniLine, 5000, (const char **) &ptr);
				if (_len == (size_t) -1) {
					GF_LOG(GF_LOG_WARNING, GF_LOG_PARSER, ("[TXTIn] Invalid UTF data (line %d)\n", ctx->curLine));
					ctx->state = 0;
				}
				len = (u32) _len;
			}
			i=j=0;
			rem_styles = 0;
			rem_color = 0;
			while (i<len) {
				u32 font_style = 0;
				u32 style_nb_chars = 0;
				u32 style_def_type = 0;

				if ( (uniLine[i]=='<') && (uniLine[i+2]=='>')) {
					style_nb_chars = 3;
					style_def_type = 1;
				}
				else if ( (uniLine[i]=='<') && (uniLine[i+1]=='/') && (uniLine[i+3]=='>')) {
					style_def_type = 2;
					style_nb_chars = 4;
				}
				else if (uniLine[i]=='<')  {
					const unsigned short* src = uniLine + i;
					size_t alen = gf_utf8_wcstombs(szLine, 2048, (const unsigned short**) & src);
					szLine[alen] = 0;
					strlwr(szLine);
					if (!strncmp(szLine, "<font ", 6) ) {
						char *a_sep = strstr(szLine, "color");
						if (a_sep) a_sep = strchr(a_sep, '"');
						if (a_sep) {
							char *e_sep = strchr(a_sep+1, '"');
							if (e_sep) {
								e_sep[0] = 0;
								font_style = gf_color_parse(a_sep+1);
								e_sep[0] = '"';
								e_sep = strchr(e_sep+1, '>');
								if (e_sep) {
									style_nb_chars = (u32) (1 + e_sep - szLine);
									style_def_type = 1;
								}
							}

						}
					}
					else if (!strncmp(szLine, "</font>", 7) ) {
						style_nb_chars = 7;
						style_def_type = 2;
						font_style = 0xFFFFFFFF;
					}


					else if (!strncmp(szLine, "<strike>", 8) ) {
						style_nb_chars = 8;
						style_def_type = 1;
					}
					else if (!strncmp(szLine, "</strike>", 9) ) {
						style_nb_chars = 9;
						style_def_type = 2;
						font_style = 0xFFFFFFFF;
					}
					//skip unknown
					else {
						char *a_sep = strstr(szLine, ">");
						if (a_sep) {
							style_nb_chars = (u32) (1 + a_sep - szLine);
							i += style_nb_chars;
							continue;
						}
					}

				}

				/*start of new style*/
				if (style_def_type==1)  {
					/*store prev style*/
					if (set_end_char) {
						assert(set_start_char);
						gf_isom_text_add_style(ctx->samp, &ctx->style);
						set_end_char = set_start_char = GF_FALSE;
						ctx->style.style_flags &= ~rem_styles;
						rem_styles = 0;
						if (rem_color) {
							ctx->style.text_color = ctx->default_color;
							rem_color = 0;
						}
					}
					if (set_start_char && (ctx->style.startCharOffset != j)) {
						ctx->style.endCharOffset = char_len + j;
						if (ctx->style.style_flags) gf_isom_text_add_style(ctx->samp, &ctx->style);
					}
					switch (uniLine[i+1]) {
					case 'b':
					case 'B':
						ctx->style.style_flags |= GF_TXT_STYLE_BOLD;
						set_start_char = GF_TRUE;
						ctx->style.startCharOffset = char_len + j;
						break;
					case 'i':
					case 'I':
						ctx->style.style_flags |= GF_TXT_STYLE_ITALIC;
						set_start_char = GF_TRUE;
						ctx->style.startCharOffset = char_len + j;
						break;
					case 'u':
					case 'U':
						ctx->style.style_flags |= GF_TXT_STYLE_UNDERLINED;
						set_start_char = GF_TRUE;
						ctx->style.startCharOffset = char_len + j;
						break;
					case 's':
					case 'S':
						ctx->style.style_flags |= GF_TXT_STYLE_STRIKETHROUGH;
						set_start_char = GF_TRUE;
						ctx->style.startCharOffset = char_len + j;
						break;
					case 'f':
					case 'F':
						if (font_style) {
							ctx->style.text_color = font_style;
							set_start_char = GF_TRUE;
							ctx->style.startCharOffset = char_len + j;
						}
						break;
					}
					i += style_nb_chars;
					continue;
				}

				/*end of prev style*/
				if (style_def_type==2)  {
					switch (uniLine[i+2]) {
					case 'b':
					case 'B':
						rem_styles |= GF_TXT_STYLE_BOLD;
						set_end_char = GF_TRUE;
						ctx->style.endCharOffset = char_len + j;
						break;
					case 'i':
					case 'I':
						rem_styles |= GF_TXT_STYLE_ITALIC;
						set_end_char = GF_TRUE;
						ctx->style.endCharOffset = char_len + j;
						break;
					case 'u':
					case 'U':
						rem_styles |= GF_TXT_STYLE_UNDERLINED;
						set_end_char = GF_TRUE;
						ctx->style.endCharOffset = char_len + j;
						break;
					case 's':
					case 'S':
						rem_styles |= GF_TXT_STYLE_STRIKETHROUGH;
						set_end_char = GF_TRUE;
						ctx->style.endCharOffset = char_len + j;
						break;
					case 'f':
					case 'F':
						if (font_style) {
							rem_color = 1;
							set_end_char = GF_TRUE;
							ctx->style.endCharOffset = char_len + j;
						}
					}
					i+=style_nb_chars;
					continue;
				}
				/*store style*/
				if (set_end_char) {
					gf_isom_text_add_style(ctx->samp, &ctx->style);
					set_end_char = GF_FALSE;
					set_start_char = GF_TRUE;
					ctx->style.startCharOffset = char_len + j;
					ctx->style.style_flags &= ~rem_styles;
					rem_styles = 0;
					ctx->style.text_color = ctx->default_color;
					rem_color = 0;
				}

				uniText[j] = uniLine[i];
				j++;
				i++;
			}
			/*store last style*/
			if (set_end_char) {
				gf_isom_text_add_style(ctx->samp, &ctx->style);
				set_end_char = GF_FALSE;
				set_start_char = GF_TRUE;
				ctx->style.startCharOffset = char_len + j;
				ctx->style.style_flags &= ~rem_styles;
			}

			char_line = j;
			uniText[j] = 0;

			sptr = (u16 *) uniText;
			len = (u32) gf_utf8_wcstombs(szText, 5000, (const u16 **) &sptr);

			gf_isom_text_add_text(ctx->samp, szText, len);
			char_len += char_line;
			txt_line ++;
			break;
		}

		if (gf_filter_pid_would_block(ctx->opid))
			return GF_OK;
	}

	/*final flush*/	
	if (ctx->end && ! ctx->noflush) {
		gf_isom_text_reset(ctx->samp);
		txtin_process_send_text_sample(ctx, ctx->samp, ctx->end, 0, GF_TRUE);
		ctx->end = 0;
	}
	gf_isom_text_reset(ctx->samp);

	return GF_EOS;
}