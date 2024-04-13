static int tcos_get_serialnr(sc_card_t *card, sc_serial_number_t *serial)
{
	int r;

	if (!serial)
		return SC_ERROR_INVALID_ARGUMENTS;

	/* see if we have cached serial number */
	if (card->serialnr.len) {
		memcpy(serial, &card->serialnr, sizeof(*serial));
		return SC_SUCCESS;
	}

	card->serialnr.len = sizeof card->serialnr.value;
	r = sc_parse_ef_gdo(card, card->serialnr.value, &card->serialnr.len, NULL, 0);
	if (r < 0) {
		card->serialnr.len = 0;
		return r;
	}

	/* copy and return serial number */
	memcpy(serial, &card->serialnr, sizeof(*serial));

	return SC_SUCCESS;
}