void crypto_cert_dns_names_free(int count, int* lengths, char** dns_names)
{
	free(lengths);

	if (dns_names)
	{
		int i;

		for (i = 0; i < count; i++)
		{
			if (dns_names[i])
			{
				OPENSSL_free(dns_names[i]);
			}
		}

		free(dns_names);
	}
}