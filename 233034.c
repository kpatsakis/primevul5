char** crypto_cert_get_dns_names(X509* x509, int* count, int** lengths)
{
	int i;
	char** result = 0;
	string_list list;
	string_list_initialize(&list);
	map_subject_alt_name(x509, GEN_DNS, extract_string, &list);
	(*count) = list.count;

	if (list.count == 0)
	{
		string_list_free(&list);
		return NULL;
	}

	/* lengths are not useful,  since we converted the
	   strings to utf-8,  there cannot be nul-bytes in them. */
	result = calloc(list.count, sizeof(*result));
	(*lengths) = calloc(list.count, sizeof(**lengths));

	if (!result || !(*lengths))
	{
		string_list_free(&list);
		free(result);
		free(*lengths);
		(*lengths) = 0;
		(*count) = 0;
		return NULL;
	}

	for (i = 0; i < list.count; i++)
	{
		result[i] = list.strings[i];
		(*lengths)[i] = strlen(result[i]);
	}

	string_list_free(&list);
	return result;
}