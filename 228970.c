print_lan_set_auth_usage(void)
{
	lprintf(LOG_NOTICE,
"lan set <channel> auth <level> <type,type,...>");
	lprintf(LOG_NOTICE,
"  level = CALLBACK, USER, OPERATOR, ADMIN");
	lprintf(LOG_NOTICE,
"  types = NONE, MD2, MD5, PASSWORD, OEM");
	lprintf(LOG_NOTICE,
"example: lan set 7 auth ADMIN PASSWORD,MD5");
}