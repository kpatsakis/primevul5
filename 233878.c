ms_escher_clientanchor (GString *buf, SheetObjectAnchor const *anchor)
{
	guint8 tmp[26] = {
		0, 0, 0x10, 0xf0, 0xde, 0xad, 0xbe, 0xef,
		0, 0,
		0,0, 0,0,
		0,0, 0,0,
		0,0, 0,0,
		0,0, 0,0
	};
	guint8 *p = tmp + 10;
	GSF_LE_SET_GUINT32 (tmp + 4, sizeof (tmp) - 8);

	GSF_LE_SET_GUINT16 (p +  0, anchor->cell_bound.start.col);
	GSF_LE_SET_GUINT16 (p +  2, (guint16)(anchor->offset[0]*1024. + .5));
	GSF_LE_SET_GUINT16 (p +  4, anchor->cell_bound.start.row);
	GSF_LE_SET_GUINT16 (p +  6, (guint16)(anchor->offset[1]*256. + .5));
	GSF_LE_SET_GUINT16 (p +  8, anchor->cell_bound.end.col);
	GSF_LE_SET_GUINT16 (p + 10, (guint16)(anchor->offset[2]*1024. + .5));
	GSF_LE_SET_GUINT16 (p + 12, anchor->cell_bound.end.row);
	GSF_LE_SET_GUINT16 (p + 14, (guint16)(anchor->offset[3]*256. + .5));
	g_string_append_len (buf, tmp, sizeof tmp);
}