iasecc_select_aid(struct sc_card *card, struct sc_aid *aid, unsigned char *out, size_t *out_len)
{
	struct sc_apdu apdu;
	unsigned char apdu_resp[SC_MAX_APDU_BUFFER_SIZE];
	int rv;

	/* Select application (deselect previously selected application) */
	sc_format_apdu(card, &apdu, SC_APDU_CASE_4_SHORT, 0xA4, 0x04, 0x00);
	apdu.lc = aid->len;
	apdu.data = aid->value;
	apdu.datalen = aid->len;
	apdu.resplen = sizeof(apdu_resp);
	apdu.resp = apdu_resp;

	rv = sc_transmit_apdu(card, &apdu);
	LOG_TEST_RET(card->ctx, rv, "APDU transmit failed");
	rv = sc_check_sw(card, apdu.sw1, apdu.sw2);
	LOG_TEST_RET(card->ctx, rv, "Cannot select AID");

	if (*out_len < apdu.resplen)
		LOG_TEST_RET(card->ctx, SC_ERROR_BUFFER_TOO_SMALL, "Cannot select AID");
	memcpy(out, apdu.resp, apdu.resplen);

	return SC_SUCCESS;
}