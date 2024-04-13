print_lan_set_cipher_privs_usage(void)
{
	lprintf(LOG_NOTICE,
"lan set <channel> cipher_privs XXXXXXXXXXXXXXX");
	lprintf(LOG_NOTICE,
"    X = Cipher Suite Unused");
	lprintf(LOG_NOTICE,
"    c = CALLBACK");
	lprintf(LOG_NOTICE,
"    u = USER");
	lprintf(LOG_NOTICE,
"    o = OPERATOR");
	lprintf(LOG_NOTICE,
"    a = ADMIN");
	lprintf(LOG_NOTICE,
"    O = OEM");
	lprintf(LOG_NOTICE,
"");
}