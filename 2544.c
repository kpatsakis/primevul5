static GF_Err ttml_setup_intervals(GF_TXTIn *ctx)
{
	u32 k, i, nb_divs;
	s64 ttml_zero_ms = 0;
	GF_Err e;
	GF_XMLNode *root;

	if (!ctx->intervals)
		ctx->intervals = gf_list_new();
	else
		ttml_reset_intervals(ctx);

	ctx->has_images = GF_FALSE;
	root = ctx->root_working_copy;
	for (k=0; k<gf_list_count(root->content); k++) {
		GF_XMLNode *head = (GF_XMLNode*)gf_list_get(root->content, k);
		if (head->type) continue;
		if (strcmp(head->name, "head")) continue;
		ttml_push_resources(ctx, NULL, head, NULL);
		break;
	}

	root = gf_xml_dom_get_root(ctx->parser);
	if (ctx->ttml_zero) {
		if (ctx->ttml_zero[0]=='T')
			ttml_zero_ms = ttml_get_timestamp(ctx, (char *) ctx->ttml_zero+1);
		else
			ttml_zero_ms = ttml_get_timestamp(ctx, (char *) ctx->ttml_zero);
	}

	nb_divs = gf_list_count(ctx->div_nodes_list);
	for (i=0; i<nb_divs; i++) {
		u32 nb_children;
		GF_XMLNode *div_node = gf_list_get(ctx->div_nodes_list, i);
		nb_children = gf_list_count(div_node->content);

		for (k=0; k<nb_children; k++) {
			TTMLInterval *ival=NULL;
			u32 p_idx;
			Bool drop = GF_FALSE;
			GF_XMLAttribute *p_att;
			GF_XMLNode *p_node;
			s64 begin=-1, end=-1;
			GF_XMLNode *adiv_child = (GF_XMLNode*)gf_list_get(div_node->content, k);
			if (adiv_child->type) continue;
			e = gf_xml_get_element_check_namespace(adiv_child, "p", root->ns);
			if (e) continue;

			p_idx = 0;
			while ( (p_att = (GF_XMLAttribute*)gf_list_enum(adiv_child->attributes, &p_idx))) {
				if (!strcmp(p_att->name, "begin")) {
					e = ttml_rewrite_timestamp(ctx, ttml_zero_ms, p_att, &begin, &drop);
					if (e) return e;
				}
				if (!strcmp(p_att->name, "end")) {
					e = ttml_rewrite_timestamp(ctx, ttml_zero_ms, p_att, &end, &drop);
					if (e) return e;
				}
			}
			if (drop) {
				gf_xml_dom_node_del(adiv_child);
				gf_list_rem(div_node->content, k);
				k--;
				nb_children--;
				continue;
			}

			e = ttml_push_interval(ctx, begin, end, &ival);
			if (e) return e;

			e = ttml_push_resources(ctx, ival, adiv_child, NULL);
			if (e) return e;

			p_idx = 0;
			while ( (p_node = (GF_XMLNode*)gf_list_enum(adiv_child->content, &p_idx))) {
				s64 s_begin=-1, s_end=-1;
				e = gf_xml_get_element_check_namespace(p_node, "span", root->ns);
				if (e) continue;

				u32 span_idx = 0;
				GF_XMLAttribute *span_att;
				while ( (span_att = (GF_XMLAttribute*)gf_list_enum(p_node->attributes, &span_idx))) {
					if (!strcmp(span_att->name, "begin")) {
						e = ttml_rewrite_timestamp(ctx, ttml_zero_ms, span_att, &s_begin, &drop);
						if (e) return e;
					} else if (!strcmp(span_att->name, "end")) {
						e = ttml_rewrite_timestamp(ctx, ttml_zero_ms, span_att, &s_end, &drop);
						if (e) return e;
					}
				}
				e = ttml_push_interval(ctx, s_begin, s_end, &ival);
				if (e) return e;

				e = ttml_push_resources(ctx, ival, p_node, NULL);
				if (e) return e;
			}
		}
	}

	//empty doc
	if (!gf_list_count(ctx->intervals)) {
		TTMLInterval *interval;
		GF_SAFEALLOC(interval, TTMLInterval);
		interval->begin = interval->end = 0;
		gf_list_add(ctx->intervals, interval);
	}

#ifndef GPAC_DISABLE_LOG
	if (gf_log_tool_level_on(GF_LOG_PARSER, GF_LOG_DEBUG)) {
		for (k=0; k<gf_list_count(ctx->intervals); k++) {
			TTMLInterval *ival = gf_list_get(ctx->intervals, k);
			GF_LOG(GF_LOG_DEBUG, GF_LOG_PARSER, ("[TTML EBU-TTD] Interval %d: "LLU"-"LLU"\n", k+1, ival->begin, ival->end));
		}
	}
#endif
	return GF_OK;
}