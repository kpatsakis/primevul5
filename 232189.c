static int tcos_finish(sc_card_t *card)
{
	free(card->drv_data);
	return 0;
}