static void txtin_probe_duration(GF_TXTIn *ctx)
{
	GF_Fraction64 dur;
	dur.num = 0;

	if (ctx->fmt == GF_TXTIN_MODE_SWF_SVG) {
#ifndef GPAC_DISABLE_SWF_IMPORT
		u32 frame_count, frame_rate;
		gf_swf_get_duration(ctx->swf_parse, &frame_rate, &frame_count);
		if (frame_count) {
			GF_Fraction64 tdur;
			tdur.num = frame_count;
			tdur.den = frame_rate;
			gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_DURATION, &PROP_FRAC64(tdur));
			gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_PLAYBACK_MODE, &PROP_UINT(GF_PLAYBACK_MODE_FASTFORWARD ) );
		}
#endif
		return;
	}
	if ((ctx->fmt == GF_TXTIN_MODE_SRT) || (ctx->fmt == GF_TXTIN_MODE_WEBVTT)  || (ctx->fmt == GF_TXTIN_MODE_SUB)) {
		u64 pos = gf_ftell(ctx->src);
		gf_fseek(ctx->src, 0, SEEK_SET);
		while (!gf_feof(ctx->src)) {
			u64 end;
			char szLine[2048];
			char *sOK = gf_text_get_utf8_line(szLine, 2048, ctx->src, ctx->unicode_type);
			if (!sOK) break;
			REM_TRAIL_MARKS(szLine, "\r\n\t ")

			if (ctx->fmt == GF_TXTIN_MODE_SUB) {
				char szText[2048];
				u32 sframe, eframe;
				if (sscanf(szLine, "{%d}{%d}%2047s", &sframe, &eframe, szText) == 3) {
					if (ctx->fps.den)
						end = 1000 * eframe * ctx->fps.num / ctx->fps.den;
					else
						end = 1000 * eframe / 25;
					if (end > (u64) dur.num) dur.num = (s64) end;
				}
			} else {
				u32 eh, em, es, ems;
				char *start = strstr(szLine, "-->");
				if (!start) continue;
				while (start[0] && ((start[0] == ' ') || (start[0] == '\t'))) start++;

				if (sscanf(start, "%u:%u:%u,%u", &eh, &em, &es, &ems) != 4) {
					eh = 0;
					if (sscanf(szLine, "%u:%u,%u", &em, &es, &ems) != 3) {
						continue;
					}
				}
				end = (3600*eh + 60*em + es)*1000 + ems;
				if (end > (u64) dur.num) dur.num = (s64) end;
			}
		}
		gf_fseek(ctx->src, pos, SEEK_SET);
		if (dur.num) {
			dur.den = 1000;
			gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_DURATION, &PROP_FRAC64(dur));
			gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_PLAYBACK_MODE, &PROP_UINT(GF_PLAYBACK_MODE_FASTFORWARD ) );
		}
		return;
	}
	if ((ctx->fmt == GF_TXTIN_MODE_TTXT) || (ctx->fmt == GF_TXTIN_MODE_TEXML)) {
		u32 i=0;
		GF_XMLNode *node, *root = gf_xml_dom_get_root(ctx->parser);
		while ((node = gf_list_enum(root->content, &i))) {
			u32 j;
			u64 duration;
			GF_XMLAttribute *att;
			if (node->type) {
				continue;
			}
			/*sample text*/
			if ((ctx->fmt == GF_TXTIN_MODE_TTXT) && strcmp(node->name, "TextSample")) continue;
			else if ((ctx->fmt == GF_TXTIN_MODE_TEXML) && strcmp(node->name, "sample")) continue;


			j=0;
			while ( (att=(GF_XMLAttribute*)gf_list_enum(node->attributes, &j))) {
				u32 h, m, s, ms;
				u64 ts=0;
				if (ctx->fmt == GF_TXTIN_MODE_TTXT) {
					if (strcmp(att->name, "sampleTime")) continue;

					if (sscanf(att->value, "%u:%u:%u.%u", &h, &m, &s, &ms) == 4) {
						ts = (h*3600 + m*60 + s)*1000 + ms;
					} else {
						ts = (u32) (atof(att->value) * 1000);
					}
					if (ts > (u64) dur.num) dur.num = (s64) ts;
				} else {
					if (strcmp(att->name, "duration")) continue;
					duration = atoi(att->value);
					dur.num += (s32) ( (1000 * duration) / ctx->txml_timescale);
				}
			}
		}
		if (dur.num) {
			dur.den = 1000;
			gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_DURATION, &PROP_FRAC64(dur));
			gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_PLAYBACK_MODE, &PROP_UINT(GF_PLAYBACK_MODE_FASTFORWARD ) );
		}
		return;
	}

	if (ctx->fmt == GF_TXTIN_MODE_TTML) {
		u32 i=0, k=0;
		GF_XMLNode *div_node;

		while ((div_node = gf_list_enum(ctx->div_nodes_list, &k))) {
			GF_XMLNode *node;
			while ((node = gf_list_enum(div_node->content, &i))) {
				GF_XMLNode *p_node;
				GF_XMLAttribute *att;
				u32 h, m, s, ms, p_idx=0;
				u64 ts_end=0;
				h = m = s = ms = 0;
				while ( (att = (GF_XMLAttribute*)gf_list_enum(node->attributes, &p_idx))) {
					if (strcmp(att->name, "end")) continue;

					if (sscanf(att->value, "%u:%u:%u.%u", &h, &m, &s, &ms) == 4) {
						ts_end = (h*3600 + m*60+s)*1000+ms;
					} else if (sscanf(att->value, "%u:%u:%u", &h, &m, &s) == 3) {
						ts_end = (h*3600 + m*60+s)*1000;
					}
				}
				//or under a <span>
				p_idx = 0;
				while ( (p_node = (GF_XMLNode*)gf_list_enum(node->content, &p_idx))) {
					u32 span_idx = 0;
					while ( (att = (GF_XMLAttribute*)gf_list_enum(p_node->attributes, &span_idx))) {
						if (strcmp(att->name, "end")) continue;
						if (sscanf(att->value, "%u:%u:%u.%u", &h, &m, &s, &ms) == 4) {
							ts_end = (h*3600 + m*60+s)*1000+ms;
						} else if (sscanf(att->value, "%u:%u:%u", &h, &m, &s) == 3) {
							ts_end = (h*3600 + m*60+s)*1000;
						}
					}
				}
				if (ts_end > (u64) dur.num) dur.num = (s64) ts_end;
			}
		}
		if (dur.num) {
			dur.den = 1000;
			gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_DURATION, &PROP_FRAC64(dur));
			gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_PLAYBACK_MODE, &PROP_UINT(GF_PLAYBACK_MODE_FASTFORWARD ) );
		}
		return;
	}
	GF_LOG(GF_LOG_WARNING, GF_LOG_PARSER, ("[TXTIn] Duration probing not supported for format %d\n", ctx->fmt));
}