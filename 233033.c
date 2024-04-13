char* crypto_cert_subject_common_name(X509* xcert, int* length)
{
	int index;
	BYTE* common_name_raw;
	char* common_name;
	X509_NAME* subject_name;
	X509_NAME_ENTRY* entry;
	ASN1_STRING* entry_data;
	subject_name = X509_get_subject_name(xcert);

	if (subject_name == NULL)
		return NULL;

	index = X509_NAME_get_index_by_NID(subject_name, NID_commonName, -1);

	if (index < 0)
		return NULL;

	entry = X509_NAME_get_entry(subject_name, index);

	if (entry == NULL)
		return NULL;

	entry_data = X509_NAME_ENTRY_get_data(entry);

	if (entry_data == NULL)
		return NULL;

	*length = ASN1_STRING_to_UTF8(&common_name_raw, entry_data);

	if (*length < 0)
		return NULL;

	common_name = _strdup((char*)common_name_raw);
	OPENSSL_free(common_name_raw);
	return (char*)common_name;
}