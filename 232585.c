get_mac_fio_flags(char_u *ptr)
{
    if ((enc_utf8 || STRCMP(p_enc, "latin1") == 0)
				     && (enc_canon_props(ptr) & ENC_MACROMAN))
	return FIO_MACROMAN;
    return 0;
}