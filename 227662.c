static u8 tdme_channelinit(u8 channel, void *device_ref)
{
	/* Transceiver front-end local oscillator tx two-point calibration
	 * value. Tuned for the hardware.
	 */
	u8 txcalval;

	if (channel >= 25)
		txcalval = 0xA7;
	else if (channel >= 23)
		txcalval = 0xA8;
	else if (channel >= 22)
		txcalval = 0xA9;
	else if (channel >= 20)
		txcalval = 0xAA;
	else if (channel >= 17)
		txcalval = 0xAB;
	else if (channel >= 16)
		txcalval = 0xAC;
	else if (channel >= 14)
		txcalval = 0xAD;
	else if (channel >= 12)
		txcalval = 0xAE;
	else
		txcalval = 0xAF;

	return tdme_setsfr_request_sync(
		1,
		CA8210_SFR_LOTXCAL,
		txcalval,
		device_ref
	);  /* LO Tx Cal */
}