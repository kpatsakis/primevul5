ms_escher_read_container (MSEscherState *state, MSEscherHeader *container,
			  gint prefix, gboolean return_attrs_in_container)
{
	MSEscherHeader h;

	g_return_val_if_fail (container != NULL, TRUE);

	ms_escher_header_init (&h);
	h.container = container;

	/* Skip the common header */
	h.offset = container->offset + prefix + COMMON_HEADER_LEN;

	do {
		guint16 tmp;
		char const *fbt_name = NULL;
		gboolean (*handler)(MSEscherState *state,
				    MSEscherHeader *container) = NULL;
		gboolean needs_free;

		guint8 const *data = ms_escher_get_data (state, h.offset,
			COMMON_HEADER_LEN, &needs_free);

		if (!data) {
			ms_escher_header_release (&h);
			return TRUE;
		}

		tmp	= GSF_LE_GET_GUINT16 (data + 0);
		h.fbt	= GSF_LE_GET_GUINT16 (data + 2);

		/* Include the length of this header in the record size */
		h.len	   = GSF_LE_GET_GUINT32 (data + 4) + COMMON_HEADER_LEN;
		h.ver      = tmp & 0x0f;
		h.instance = (tmp >> 4) & 0xfff;

		d (0 , g_printerr ("length 0x%x(=%d), ver 0x%x, instance 0x%x, offset = 0x%x(=%d);\n",
			       h.len, h.len, h.ver, h.instance, h.offset, h.offset););

		if (needs_free)
			g_free ((void *)data);

		/*
		 * Let's double check that the data we just read makes sense.
		 * If problems arise in the next tests it probably indicates
		 * that the PRECEDING record length was invalid.  Check that
		 * it included the header.
		 */
		if ((h.fbt & (~0x1ff)) != 0xf000) {
			g_warning ("Invalid fbt = 0x%x\n", h.fbt);
			ms_escher_header_release (&h);
			return TRUE;
		}

#define EshRecord(x) \
		x : fbt_name = #x; \
		    handler = &ms_escher_read_ ## x; \
		break

		switch (h.fbt) {
		case EshRecord(DggContainer);	case EshRecord(Dgg);
		case EshRecord(DgContainer);	case EshRecord(Dg);
		case EshRecord(SpgrContainer);	case EshRecord(Spgr);
		case EshRecord(SpContainer);	case EshRecord(Sp);
		case EshRecord(BStoreContainer);case EshRecord(BSE);
		case EshRecord(Textbox);	case EshRecord(ClientTextbox);
		case EshRecord(Anchor); case EshRecord(ChildAnchor); case EshRecord(ClientAnchor);
		case EshRecord(ClientData);
		case EshRecord(CLSID);
		case EshRecord(OPT);
		case EshRecord(ColorMRU);
		case EshRecord(SplitMenuColors);
		case EshRecord(RegroupItems);
		case EshRecord(ColorScheme);
		case EshRecord(OleObject);
		case EshRecord(DeletedPspl);
		case EshRecord(SolverContainer);
		case EshRecord(ConnectorRule);
		case EshRecord(AlignRule);
		case EshRecord(ArcRule);
		case EshRecord(ClientRule);
		case EshRecord(CalloutRule);
		case EshRecord(Selection);
		case EshRecord(UserDefined);
		default : fbt_name = NULL;
		}
#undef EshRecord

		if (Blip_START <= h.fbt && h.fbt <= Blip_END) {
			ms_escher_read_Blip (state, &h);
		} else if (fbt_name != NULL) {
			gboolean res;

			/* Not really needed */
			g_return_val_if_fail (handler != NULL, TRUE);

			d (0, g_printerr ("{ /* %s */\n", fbt_name););
			res = (*handler)(state, &h);

			d (0, g_printerr ("}; /* %s */\n", fbt_name););
			if (res) {
				ms_escher_header_release (&h);
				g_warning ("%s;", fbt_name);
				return TRUE;
			}

		} else
			g_warning ("Invalid fbt = %x;", h.fbt);

		h.offset += h.len;
	} while (h.offset < (container->offset + container->len));

	if (container->attrs == NULL && return_attrs_in_container) {
		container->attrs = h.attrs;
		h.release_attrs = FALSE;
	}
	ms_escher_header_release (&h);
	return FALSE;
}