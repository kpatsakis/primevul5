ms_escher_read_OPT_bools (MSEscherHeader *h,
			  MSEscherBoolOptTable const *bools, unsigned n_bools,
			  guint pid, guint32 const val)
{
	unsigned i;
	guint32 mask = 0x10000 << (n_bools - 1);
	guint32 bit  = 0x00001 << (n_bools - 1);

	g_return_if_fail (n_bools > 0);
	g_return_if_fail (bools[n_bools-1].pid == pid);

	d (2, g_printerr ("Set of Bools %d-%d = 0x%08x;\n{\n",
		      bools[0].pid, bools[n_bools-1].pid, val););

	pid -= (n_bools - 1);
	for (i = 0; i < n_bools; i++, mask >>= 1, bit >>= 1, pid++) {
		gboolean set_val = ((val & bit) == bit);
		gboolean def_val = bools[i].default_val;
		const MSObjAttrID aid = bools[i].id;

		if (!(val & mask))	/* the value is set */
			continue;

		d (0, g_printerr ("bool %s(%d) = %s; /* def: %s; gnm: %d */\n",
			      bools[i].name, pid,
			      set_val ? "true" : "false",
			      def_val ? "true" : "false",
			      aid););

		if (set_val != def_val && aid != 0)
			ms_escher_header_add_attr
				(h, ms_obj_attr_new_flag (aid));
	}
	d (2, g_printerr ("};\n"););
}