rpa_add_realm(string_t *realms, const char *realm, const char *service)
{
	str_append(realms, service);	
	str_append_c(realms, '@');
	str_append(realms, realm);
	str_append_c(realms, ' ');
}