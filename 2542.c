static GF_Err gf_text_process_ttml(GF_Filter *filter, GF_TXTIn *ctx)
{
	GF_Err e;
	GF_XMLNode *root;
	u32 i, nb_res_interval=0, k, nb_div_nodes;
	char *samp_text=NULL;
	GF_List *emb_resources = NULL;
	TTMLInterval *interval;
	Bool sample_empty = GF_TRUE;

	if (!ctx->is_setup) return gf_text_ttml_setup(filter, ctx);
	if (ctx->non_compliant_ttml || !ctx->opid) return GF_NOT_SUPPORTED;
	if (!ctx->playstate) return GF_OK;
	else if (ctx->playstate==2) return GF_EOS;

	if (ctx->seek_state==1) {
		ctx->seek_state = 2;
		ctx->current_tt_interval = 0;
	}

	interval = gf_list_get(ctx->intervals, ctx->current_tt_interval);
	if (!interval) {
		GF_LOG(GF_LOG_DEBUG, GF_LOG_PARSER, ("[TTML EBU-TTD] last_sample_duration="LLU", last_sample_end="LLU"\n", ctx->last_sample_duration, ctx->end));

		gf_filter_pid_set_info_str( ctx->opid, "ttxt:last_dur", &PROP_UINT((u32) ctx->last_sample_duration) );
		gf_filter_pid_set_eos(ctx->opid);
		return GF_EOS;
	}
	ctx->current_tt_interval++;

	emb_resources = interval->resources ? interval->resources : ctx->ttml_resources;
	nb_res_interval = gf_list_count(emb_resources);

	root = gf_xml_dom_get_root(ctx->parser);

	nb_div_nodes = gf_list_count(ctx->div_nodes_list);
	for (k=0; k<nb_div_nodes; k++) {
		Bool has_content = GF_FALSE;
		GF_XMLNode *div_node = gf_list_get(ctx->div_nodes_list, k);
		u32 nb_children = gf_list_count(div_node->content);

		GF_XMLNode *copy_div_node = gf_list_get(ctx->body_node->content, k);

		for (i=0; i < nb_children; i++) {
			GF_XMLNode *p_node;
			GF_XMLAttribute *p_att;
			u32 p_idx = 0;
			s64 ts_begin = -1, ts_end = -1;
			Bool in_range;
			GF_XMLNode *div_child = (GF_XMLNode*)gf_list_get(div_node->content, i);
			if (div_child->type) {
				continue;
			}
			e = gf_xml_get_element_check_namespace(div_child, "p", root->ns);
			if (e == GF_BAD_PARAM) {
				GF_LOG(GF_LOG_WARNING, GF_LOG_PARSER, ("[TTML EBU-TTD] ignored \"%s\" node, check your namespaces\n", div_child->name));
				continue;
			}


			//sample is either in the <p> ...
			p_idx = 0;
			while ( (p_att = (GF_XMLAttribute*)gf_list_enum(div_child->attributes, &p_idx))) {
				if (!strcmp(p_att->name, "begin")) {
					if (ts_begin != -1) {
						GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[TTML EBU-TTD] duplicated \"begin\" attribute. Abort.\n"));
						e = GF_NON_COMPLIANT_BITSTREAM;
						goto exit;
					}
					ts_begin = ttml_get_timestamp(ctx, p_att->value);
				} else if (!strcmp(p_att->name, "end")) {
					if (ts_end != -1) {
						GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[TTML EBU-TTD] duplicated \"end\" attribute. Abort.\n"));
						e = GF_NON_COMPLIANT_BITSTREAM;
						goto exit;
					}
					ts_end = ttml_get_timestamp(ctx, p_att->value);
				}
			}

			in_range = ttml_check_range(interval, ts_begin, ts_end);
			if (in_range) {
				GF_XMLNode *prev_child = i ? (GF_XMLNode*) gf_list_get(div_node->content, i-1) : NULL;
				if (prev_child && prev_child->type) {
					gf_xml_dom_append_child(copy_div_node, prev_child);
				}
				e = gf_xml_dom_append_child(copy_div_node, div_child);
				assert(e == GF_OK);
				has_content = GF_TRUE;
			}

			//or under a <span>
			p_idx = 0;
			while ( (p_node = (GF_XMLNode*)gf_list_enum(div_child->content, &p_idx))) {
				u32 span_idx = 0;
				GF_XMLAttribute *span_att;
				e = gf_xml_get_element_check_namespace(p_node, "span", root->ns);
				if (e == GF_BAD_PARAM) {
					GF_LOG(GF_LOG_WARNING, GF_LOG_PARSER, ("[TTML EBU-TTD] ignored \"%s\" node, check your namespaces\n", p_node->name));
				}
				else if (e)
					continue;

				ts_begin = ts_end = -1;
				while ( (span_att = (GF_XMLAttribute*)gf_list_enum(p_node->attributes, &span_idx))) {
					if (!strcmp(span_att->name, "begin")) {
						if (ts_begin != -1) {
							GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[TTML EBU-TTD] duplicated \"begin\" attribute under <span>. Abort.\n"));
							e = GF_NON_COMPLIANT_BITSTREAM;
							goto exit;
						}
						ts_begin = ttml_get_timestamp(ctx, span_att->value);
					} else if (!strcmp(span_att->name, "end")) {
						if (ts_end != -1) {
							GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[TTML EBU-TTD] duplicated \"end\" attribute under <span>. Abort.\n"));
							e = GF_NON_COMPLIANT_BITSTREAM;
							goto exit;
						}
						ts_end = ttml_get_timestamp(ctx, span_att->value);
					}
				}

				/*append the entire <p> and break (we cannot split the text content)*/
				in_range = ttml_check_range(interval, ts_begin, ts_end);
				if (in_range) {
					GF_XMLNode *prev_child = i ? (GF_XMLNode*) gf_list_get(div_node->content, i-1) : NULL;
					if (prev_child && prev_child->type) {
						gf_xml_dom_append_child(copy_div_node, prev_child);
					}
					e = gf_xml_dom_append_child(copy_div_node, div_child);
					assert(e == GF_OK);
					has_content = GF_TRUE;
					break;
				}
			}
		}
		if (has_content) {
			GF_XMLNode *last_child = (GF_XMLNode*) gf_list_last(div_node->content);
			if (last_child && last_child->type) {
				gf_xml_dom_append_child(copy_div_node, last_child);
			}
			sample_empty = GF_FALSE;
		}
	}

	//empty doc
	if (!ctx->body_node)
		sample_empty = GF_FALSE;

	if (! sample_empty) {
		samp_text = gf_xml_dom_serialize_root((GF_XMLNode*)ctx->root_working_copy, GF_FALSE, GF_FALSE);

		for (k=0; k<nb_div_nodes; k++) {
			GF_XMLNode *copy_div_node = gf_list_get(ctx->body_node->content, k);
			if (!copy_div_node->type)
				gf_list_reset(copy_div_node->content);
		}
	}

	if (samp_text) {
		GF_FilterPacket *pck;
		u8 *pck_data;
		Bool skip_pck = GF_FALSE;
		u32 txt_len;
		u32 res_len = 0;
		char *txt_str;


		if (interval->begin < (s64) ctx->end) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[TTML EBU-TTD] Error computing overlapped intervals! \"begin\" is "LLD" , last \"end\" was "LLD". Abort.\n", interval->begin, ctx->end));
			e = GF_NOT_SUPPORTED;
			goto exit;
		}

		txt_str = ttxt_parse_string(samp_text, GF_TRUE);
		if (!txt_str) txt_str = "";
		txt_len = (u32) strlen(txt_str);

		for (i=0; i<nb_res_interval; i++) {
			TTMLRes *res = gf_list_get(emb_resources, i);
			res_len += res->size;
		}

		if (ctx->first_samp) {
			interval->begin = 0; /*in MP4 we must start at T=0*/
			ctx->first_samp = GF_FALSE;
		}

		ctx->last_sample_duration = interval->end - interval->begin;

		ctx->end = interval->end;
		GF_LOG(GF_LOG_DEBUG, GF_LOG_PARSER, ("ts_begin="LLD", ts_end="LLD", last_sample_duration="LLU" (real duration: "LLU"), last_sample_end="LLU"\n", interval->begin, interval->end, interval->end - ctx->end, ctx->last_sample_duration, ctx->end));

		if (ctx->seek_state==2) {
			Double end = (Double) interval->end;
			end /= ctx->timescale;
			if (end<ctx->start_range) skip_pck = GF_TRUE;
			else ctx->seek_state = 0;
		}

		if (!skip_pck) {
			pck = gf_filter_pck_new_alloc(ctx->opid, txt_len+res_len, &pck_data);
			if (!pck) {
				gf_free(samp_text);
				return GF_OUT_OF_MEM;
			}
			memcpy(pck_data, txt_str, txt_len);
			gf_filter_pck_set_sap(pck, GF_FILTER_SAP_1);

			if (ctx->ttml_dur>0) {
				gf_filter_pck_set_cts(pck, 0);
				gf_filter_pck_set_duration(pck, (u32) ctx->ttml_dur);
				ctx->last_sample_duration = (u64) ctx->ttml_dur * 1000 / ctx->timescale;
			} else {
				gf_filter_pck_set_cts(pck, (ctx->timescale * interval->begin)/1000);
				if (interval->end >= interval->begin) {
					gf_filter_pck_set_duration(pck, (u32) ((ctx->timescale * (interval->end - interval->begin) )/1000) );
				}
			}

			if (res_len) {
				GF_BitStream *subs = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);
				u8 *subs_data;
				u32 subs_size;
				//subs 0
				gf_bs_write_u32(subs, 0);
				gf_bs_write_u32(subs, txt_len);
				gf_bs_write_u32(subs, 0);
				gf_bs_write_u8(subs, 0);
				gf_bs_write_u8(subs, 0);

				pck_data += txt_len;
				for (i=0; i<nb_res_interval; i++) {
					TTMLRes *res = gf_list_get(emb_resources, i);
					memcpy(pck_data, res->data, res->size);
					pck_data += res->size;

					//subs >0
					gf_bs_write_u32(subs, 0);
					gf_bs_write_u32(subs, res->size);
					gf_bs_write_u32(subs, 0);
					gf_bs_write_u8(subs, 0);
					gf_bs_write_u8(subs, 0);
				}
				gf_bs_get_content(subs, &subs_data, &subs_size);
				gf_bs_del(subs);
				gf_filter_pck_set_property(pck, GF_PROP_PCK_SUBS, &PROP_DATA_NO_COPY(subs_data, subs_size) );
			}
			gf_filter_pck_send(pck);
		}

		gf_free(samp_text);
		samp_text = NULL;
	} else {
		GF_LOG(GF_LOG_WARNING, GF_LOG_PARSER, ("[TTML EBU-TTD] empty sample (begin="LLD", end="LLD"). Skip.\n", interval->begin, interval->end));
	}

	return GF_OK;


exit:
	if (!ctx->non_compliant_ttml) {
		ctx->non_compliant_ttml = GF_TRUE;
		gf_filter_pid_set_discard(ctx->ipid, GF_TRUE);
	}
	return e;
}