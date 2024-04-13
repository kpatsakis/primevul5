static int tcos_match_card(sc_card_t *card)
{
	int i;

	i = _sc_match_atr(card, tcos_atrs, &card->type);
	if (i < 0)
		return 0;
	return 1;
}