unsigned char	*kssl_skip_confound(krb5_enctype etype, unsigned char *a)
	{
	int 		i, conlen;
	size_t		cklen;
	static size_t 	*cksumlens = NULL;
	unsigned char	*test_auth;

	conlen = (etype)? 8: 0;

	if (!cksumlens  &&  !(cksumlens = populate_cksumlens()))  return NULL;
	for (i=0; (cklen = cksumlens[i]) != 0; i++)
		{
		test_auth = a + conlen + cklen;
		if (kssl_test_confound(test_auth))  return test_auth;
		}

	return NULL;
	}