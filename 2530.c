static GF_Err gf_text_ttml_setup(GF_Filter *filter, GF_TXTIn *ctx)
{
	GF_Err e;
	u32 i, nb_children, ID;
	u64 file_size;
	s32 sub_fps_num, sub_fps_den;
	GF_XMLAttribute *att;
	GF_XMLNode *root, *node, *body_node;
	const char *lang = ctx->lang;


	ctx->is_setup = GF_TRUE;
	ctx->parser = gf_xml_dom_new();
	e = gf_xml_dom_parse(ctx->parser, ctx->file_name, ttxt_dom_progress, ctx);
	if (e) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[TXTIn] Error parsing TTML file: Line %d - %s. Abort.\n", gf_xml_dom_get_line(ctx->parser), gf_xml_dom_get_error(ctx->parser) ));
		ctx->is_setup = GF_TRUE;
		ctx->non_compliant_ttml = GF_TRUE;
		return e;
	}
	root = gf_xml_dom_get_root(ctx->parser);
	if (!root) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[TXTIn] Error parsing TTML file: no root XML element found. Abort.\n"));
		ctx->non_compliant_ttml = GF_TRUE;
		return GF_NON_COMPLIANT_BITSTREAM;
	}

	/*look for TTML*/
	if (gf_xml_get_element_check_namespace(root, "tt", NULL) != GF_OK) {
		if (root->ns) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_PARSER, ("TTML file not recognized: root element is \"%s:%s\" (check your namespaces)\n", root->ns, root->name));
		} else {
			GF_LOG(GF_LOG_WARNING, GF_LOG_PARSER, ("TTML file not recognized: root element is \"%s\"\n", root->name));
		}
		ctx->non_compliant_ttml = GF_TRUE;
		return GF_NOT_SUPPORTED;
	}

	GF_LOG(GF_LOG_DEBUG, GF_LOG_PARSER, ("[TXTIn] TTML EBU-TTD detected\n"));

	root = gf_xml_dom_get_root(ctx->parser);


	/*** root (including language) ***/
	sub_fps_num = 0;
	sub_fps_den = 0;
	i=0;
	while ( (att = (GF_XMLAttribute *)gf_list_enum(root->attributes, &i))) {
		const char *att_name;
		GF_LOG(GF_LOG_DEBUG, GF_LOG_PARSER, ("[TTML] Found root attribute name %s, value %s\n", att->name, att->value));

		att_name = strchr(att->name, ':');
		if (att_name) att_name++;
		else att_name = att->name;

		if (!strcmp(att->name, "xmlns")) {
			if (strcmp(att->value, TTML_NAMESPACE)) {
				GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[TTML] XML Namespace %s not recognized, expecting %s\n", att->name, att->value, TTML_NAMESPACE));
				ctx->non_compliant_ttml = GF_TRUE;
				return GF_NON_COMPLIANT_BITSTREAM;
			}
		}
		else if (!strcmp(att->name, "xml:lang") && att->value && strlen(att->value)) {
			lang = att->value;
		}
		else if (!strcmp(att_name, "tickRate") && att->value) {
			ctx->tick_rate = atoi(att->value);
		}
		else if (!strcmp(att_name, "frameRate") && att->value) {
			ctx->ttml_fps_num = atoi(att->value);
			ctx->ttml_fps_den = 1;
		}
		else if (!strcmp(att_name, "frameRateMultiplier") && att->value) {
			char *sep = strchr(att->value, ' ');
			if (!sep) sep = strchr(att->value, '\t');
			if (sep) {
				u8 c = sep[0];
				sep[0] = 0;
				sub_fps_num = atoi(sep);
				sep[0] = c;
				while ((sep[0]==' ') || (sep[0]=='\t'))
					sep++;
				sub_fps_den = atoi(sep);
			}
		}
		else if (!strcmp(att_name, "subFrameRate") && att->value) {
			ctx->ttml_sfps = atoi(att->value);
		}
	}

	if (sub_fps_num && sub_fps_den && ctx->ttml_fps_num) {
		ctx->ttml_fps_num *= sub_fps_num;
		ctx->ttml_fps_den = sub_fps_den;
	}

	//locate body
	nb_children = gf_list_count(root->content);
	body_node = NULL;

	i=0;
	while ( (node = (GF_XMLNode*)gf_list_enum(root->content, &i))) {
		if (node->type) {
			nb_children--;
			continue;
		}
		e = gf_xml_get_element_check_namespace(node, "body", root->ns);
		if (e == GF_BAD_PARAM) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_PARSER, ("[TTML EBU-TTD] ignored \"%s\" node, check your namespaces\n", node->name));
		} else if (e == GF_OK) {
			if (body_node) {
				GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[TTML EBU-TTD] duplicated \"body\" element. Abort.\n"));
				ctx->non_compliant_ttml = GF_TRUE;
				return GF_NON_COMPLIANT_BITSTREAM;
			}
			body_node = node;
		}
	}
	if (!body_node) {
		GF_LOG(GF_LOG_DEBUG, GF_LOG_PARSER, ("[TTML EBU-TTD] \"body\" element not found, assuming empty doc\n"));
	}

	if (!ctx->div_nodes_list) {
		ctx->div_nodes_list = gf_list_new();
		if (!ctx->div_nodes_list) return GF_OUT_OF_MEM;
	} else {
		gf_list_reset(ctx->div_nodes_list);
	}

	if (body_node) {
		i=0;
		while ( (node = (GF_XMLNode*)gf_list_enum(body_node->content, &i))) {
			if (!node->type) {
				e = gf_xml_get_element_check_namespace(node, "div", root->ns);
				if (e == GF_BAD_PARAM) {
					GF_LOG(GF_LOG_WARNING, GF_LOG_PARSER, ("[TTML EBU-TTD] ignored \"%s\" node, check your namespaces\n", node->name));
				}
			}
			gf_list_add(ctx->div_nodes_list, node);
		}
	}
	file_size = ctx->end;
	if (!ctx->timescale) ctx->timescale = 1000;

	if (!ctx->opid) ctx->opid = gf_filter_pid_new(filter);
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_STREAM_TYPE, &PROP_UINT(GF_STREAM_TEXT) );
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_CODECID, &PROP_UINT(GF_CODECID_SUBS_XML) );
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_TIMESCALE, &PROP_UINT(ctx->timescale) );
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_DOWN_SIZE, &PROP_LONGUINT(file_size) );

	ID = 1;
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_ID, &PROP_UINT(ID) );
	if (ctx->width) gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_WIDTH, &PROP_UINT(ctx->width) );
	if (ctx->height) gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_HEIGHT, &PROP_UINT(ctx->height) );
	if (ctx->zorder) gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_ZORDER, &PROP_SINT(ctx->zorder) );
	if (lang) gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_LANGUAGE, &PROP_STRING( lang) );
	gf_filter_pid_set_property_str(ctx->opid, "meta:xmlns", &PROP_STRING(TTML_NAMESPACE) );

	/*** body ***/
	ctx->parser_working_copy = gf_xml_dom_new();
	e = gf_xml_dom_parse(ctx->parser_working_copy, ctx->file_name, NULL, NULL);
	assert (e == GF_OK);
	ctx->root_working_copy = gf_xml_dom_get_root(ctx->parser_working_copy);
	assert(ctx->root_working_copy);

	if (body_node) {
		/*remove all the sample entries (instances in body) entries from the working copy, we will add each sample in this clone DOM  to create full XML of each sample*/
		ebu_ttd_remove_samples(ctx->root_working_copy, &ctx->body_node);
		if (!ctx->body_node) {
			return GF_NON_COMPLIANT_BITSTREAM;
		}
	} else {
		ctx->body_node = NULL;
	}

	ctx->current_tt_interval = 0;

	ctx->last_sample_duration = 0;
	ctx->end = 0;
	ctx->first_samp = GF_TRUE;

	txtin_probe_duration(ctx);

	e = ttml_setup_intervals(ctx);
	if (e) return e;

	if (ctx->has_images) {
		char *mime_cfg = "application/ttml+xml;codecs=im1i";
		gf_filter_pid_set_property_str(ctx->opid, "meta:mime", &PROP_STRING(mime_cfg) );
	} else {
		gf_filter_pid_set_property_str(ctx->opid, "meta:mime", NULL);
	}
	return GF_OK;
}