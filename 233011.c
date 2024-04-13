static void map_subject_alt_name(X509* x509, int general_name_type, general_name_mapper_pr mapper,
                                 void* data)
{
	int i;
	int num;
	STACK_OF(GENERAL_NAME) * gens;
	gens = X509_get_ext_d2i(x509, NID_subject_alt_name, NULL, NULL);

	if (!gens)
	{
		return;
	}

	num = sk_GENERAL_NAME_num(gens);

	for (i = 0; (i < num); i++)
	{
		GENERAL_NAME* name = sk_GENERAL_NAME_value(gens, i);

		if (name)
		{
			if ((general_name_type == GEN_ALL) || (general_name_type == name->type))
			{
				if (!mapper(name, data, i, num))
				{
					break;
				}
			}
		}
	}

	sk_GENERAL_NAME_pop_free(gens, GENERAL_NAME_free);
}