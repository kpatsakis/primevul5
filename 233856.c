ms_escher_read_ColorMRU (MSEscherState *state, MSEscherHeader *h)
{
	d (3 , {
		guint const num_Colours = h->instance;
		g_printerr ("There are %d Colours in a record with remaining length %d;\n",
			num_Colours, (h->len - COMMON_HEADER_LEN));
	});

	/* Colors in order from left to right.  */
	/* TODO : When we know how to parse a Colour record read these */
	return FALSE;
}