ms_escher_parse (BiffQuery *q, MSContainer *container, gboolean return_attrs)
{
	MSEscherState state;
	MSEscherHeader fake_header;
	MSObjAttrBag *res = NULL;
	char const *drawing_record_name = "Unknown";

	g_return_val_if_fail (q != NULL, NULL);

	if (q->opcode == BIFF_MS_O_DRAWING)
		drawing_record_name = "Drawing";
	else if (q->opcode == BIFF_MS_O_DRAWING_GROUP)
		drawing_record_name = "Drawing Group";
	else if (q->opcode == BIFF_MS_O_DRAWING_SELECTION)
		drawing_record_name = "Drawing Selection";
	else if (q->opcode == BIFF_CHART_gelframe)
		drawing_record_name = "Chart GelFrame";
	else {
		g_warning ("EXCEL : unexpected biff type %x;", q->opcode);
		return NULL;
	}

	state.container	   = container;
	state.q		   = q;
	state.segment_len  = q->length;
	state.start_offset = 0;
	state.end_offset   = q->length;

	ms_escher_header_init (&fake_header);
	fake_header.container = NULL;
	fake_header.offset = 0;

	d (0, g_printerr ("{  /* Escher '%s'*/\n", drawing_record_name););
	ms_escher_read_container (&state,
		&fake_header, -COMMON_HEADER_LEN, return_attrs);
	d (0, g_printerr ("}; /* Escher '%s'*/\n", drawing_record_name););

	if (return_attrs) {
		res = fake_header.attrs;
		fake_header.release_attrs = FALSE;
	}
	ms_escher_header_release (&fake_header);
	return res;
}