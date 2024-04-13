ms_escher_blip_free (MSEscherBlip *blip)
{
	blip->type = NULL;
	if (blip->needs_free) {
		g_free (blip->data);
		blip->needs_free = FALSE;
	}
	blip->data = NULL;
	g_free (blip);
}