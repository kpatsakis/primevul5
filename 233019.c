char* crypto_cert_get_email(X509* x509)
{
	char* result = 0;
	string_list list;
	string_list_initialize(&list);
	list.maximum = 1;
	map_subject_alt_name(x509, GEN_EMAIL, extract_string, &list);

	if (list.count == 0)
	{
		string_list_free(&list);
		return 0;
	}

	result = _strdup(list.strings[0]);
	OPENSSL_free(list.strings[0]);
	string_list_free(&list);
	return result;
}