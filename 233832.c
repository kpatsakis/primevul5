ms_escher_read_Dg (MSEscherState *state, MSEscherHeader *h)
{
#if 0
	guint8 const *data = h->data + COMMON_HEADER_LEN;
	guint32 num_shapes = GSF_LE_GET_GUINT32(data);
	/* spid_cur = last SPID given to an SP in this DG :-)  */
	guint32 spid_cur   = GSF_LE_GET_GUINT32(data+4);
	guint32 drawing_id = h->instance;

	/* This drawing has these num_shapes shapes, with a pointer to the last
	 * SPID given to it */
#endif
	return FALSE;
}