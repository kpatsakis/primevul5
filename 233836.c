ms_escher_read_Dgg (MSEscherState *state, MSEscherHeader *h)
{
#if 0
	typedef struct {
		guint32 max_spid;
		guint32 num_id_clust;
		guint32 num_shapes_saved; /* Includes deleted shapes if undo saved */
		guint32 num_drawings_saved;
		GArray *id_clusts;
	} FDGG;
	typedef struct {
		guint32 DG_owning_spids;
		guint32 spids_used_so_far;
	} ID_CLUST;

	FDGG fd;
	guint32 lp;
	guint8 const *data = h->data + COMMON_HEADER_LEN;
	fd.id_clusts = g_array_new (1, 1, sizeof(ID_CLUST));
	fd.max_spid           = GSF_LE_GET_GUINT32(data+ 0);
	fd.num_id_clust       = GSF_LE_GET_GUINT32(data+ 4);
	fd.num_shapes_saved   = GSF_LE_GET_GUINT32(data+ 8);
	fd.num_drawings_saved = GSF_LE_GET_GUINT32(data+12);

	g_printerr ("maxspid 0x%x clusts 0x%x shapes 0x%x drawings x%x\n",
		fd.max_spid, fd.num_id_clust, fd.num_shapes_saved,
		fd.num_drawings_saved);

	data+=16;
	for (lp = 0; lp < fd.num_id_clust; lp++) {
		ID_CLUST cl;
		cl.DG_owning_spids   = GSF_LE_GET_GUINT32(data+0);
		cl.spids_used_so_far = GSF_LE_GET_GUINT32(data+4);
		g_array_append_val (fd.id_clusts, cl);
	}
#endif
	return FALSE;
}