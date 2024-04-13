ms_escher_blip_new (guint8 *data, guint32 len, char const *type, gboolean copy)
{
	MSEscherBlip *blip = g_new (MSEscherBlip, 1);

	blip->type = type;
	blip->data_len = len;
	blip->needs_free = TRUE;
	blip->data = copy ? g_memdup (data, len) : data;

	return blip;
}