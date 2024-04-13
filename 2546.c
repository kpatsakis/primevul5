static GF_Err txtin_texml_setup(GF_Filter *filter, GF_TXTIn *ctx)
{
	GF_Err e;
	u32 ID, OCR_ES_ID, i;
	u64 file_size;
	GF_XMLAttribute *att;
	GF_XMLNode *root;

	ctx->parser = gf_xml_dom_new();
	e = gf_xml_dom_parse(ctx->parser, ctx->file_name, ttxt_dom_progress, ctx);
	if (e) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[TXTIn] Error parsing TeXML file: Line %d - %s", gf_xml_dom_get_line(ctx->parser), gf_xml_dom_get_error(ctx->parser) ));
		gf_xml_dom_del(ctx->parser);
		ctx->parser = NULL;
		return e;
	}

	root = gf_xml_dom_get_root(ctx->parser);

	if (strcmp(root->name, "text3GTrack")) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[TXTIn] Invalid QT TeXML file - expecting root \"text3GTrack\" got \"%s\"", root->name));
		return GF_NON_COMPLIANT_BITSTREAM;
	}
	file_size = ctx->end;
	ctx->txml_timescale = 600;

	i=0;
	while ( (att=(GF_XMLAttribute *)gf_list_enum(root->attributes, &i))) {
		if (!strcmp(att->name, "trackWidth")) ctx->width = atoi(att->value);
		else if (!strcmp(att->name, "trackHeight")) ctx->height = atoi(att->value);
		else if (!strcmp(att->name, "layer")) ctx->zorder = atoi(att->value);
		else if (!strcmp(att->name, "timeScale")) ctx->txml_timescale = atoi(att->value);
		else if (!strcmp(att->name, "transform")) {
			Float fx, fy;
			sscanf(att->value, "translate(%f,%f)", &fx, &fy);
			ctx->txtx = (u32) fx;
			ctx->txty = (u32) fy;
		}
	}

	/*setup track in 3GP format directly (no ES desc)*/
	OCR_ES_ID = ID = 0;
	if (!ctx->timescale) ctx->timescale = 1000;

	if (!ctx->opid) ctx->opid = gf_filter_pid_new(filter);
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_STREAM_TYPE, &PROP_UINT(GF_STREAM_TEXT) );
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_CODECID, &PROP_UINT(GF_ISOM_SUBTYPE_TX3G) );
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_TIMESCALE, &PROP_UINT(ctx->timescale) );
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_DOWN_SIZE, &PROP_LONGUINT(file_size) );


	if (!ID) ID = 1;
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_ID, &PROP_UINT(ID) );
	if (OCR_ES_ID) gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_CLOCK_ID, &PROP_UINT(OCR_ES_ID) );
	if (ctx->width) gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_WIDTH, &PROP_UINT(ctx->width) );
	if (ctx->height) gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_HEIGHT, &PROP_UINT(ctx->height) );
	if (ctx->zorder) gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_ZORDER, &PROP_SINT(ctx->zorder) );
	if (ctx->lang) gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_LANGUAGE, &PROP_STRING( ctx->lang) );


	ctx->nb_children = gf_list_count(root->content);
	ctx->cur_child_idx = 0;
	txtin_probe_duration(ctx);

	return GF_OK;
}