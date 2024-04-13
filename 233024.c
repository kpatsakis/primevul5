static int extract_string(GENERAL_NAME* name, void* data, int index, int count)
{
	string_list* list = data;
	unsigned char* cstring = 0;
	ASN1_STRING* str;

	switch (name->type)
	{
		case GEN_URI:
			str = name->d.uniformResourceIdentifier;
			break;

		case GEN_DNS:
			str = name->d.dNSName;
			break;

		case GEN_EMAIL:
			str = name->d.rfc822Name;
			break;

		default:
			return 1;
	}

	if ((ASN1_STRING_to_UTF8(&cstring, str)) < 0)
	{
		WLog_ERR(TAG, "ASN1_STRING_to_UTF8() failed for %s: %s",
		         general_name_type_label(name->type), ERR_error_string(ERR_get_error(), NULL));
		return 1;
	}

	string_list_allocate(list, count);

	if (list->allocated <= 0)
	{
		OPENSSL_free(cstring);
		return 0;
	}

	list->strings[list->count] = (char*)cstring;
	list->count++;

	if (list->count >= list->maximum)
	{
		return 0;
	}

	return 1;
}