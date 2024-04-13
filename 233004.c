char* crypto_cert_get_upn(X509* x509)
{
	char* result = 0;
	object_list list;
	object_list_initialize(&list);
	list.type_id = OBJ_nid2obj(NID_ms_upn);
	list.maximum = 1;
	map_subject_alt_name(x509, GEN_OTHERNAME, extract_othername_object_as_string, &list);

	if (list.count == 0)
	{
		object_list_free(&list);
		return 0;
	}

	result = list.strings[0];
	object_list_free(&list);
	return result;
}