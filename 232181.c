static int tcos_card_ctl(sc_card_t *card, unsigned long cmd, void *ptr)
{
	switch (cmd) {
	case SC_CARDCTL_TCOS_SETPERM:
		return tcos_setperm(card, !!ptr);
	case SC_CARDCTL_GET_SERIALNR:
		return tcos_get_serialnr(card, (sc_serial_number_t *)ptr);
	}
	return SC_ERROR_NOT_SUPPORTED;
}