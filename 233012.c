static char* crypto_print_name(X509_NAME* name)
{
	char* buffer = NULL;
	BIO* outBIO = BIO_new(BIO_s_mem());

	if (X509_NAME_print_ex(outBIO, name, 0, XN_FLAG_ONELINE) > 0)
	{
		unsigned long size = BIO_number_written(outBIO);
		buffer = calloc(1, size + 1);

		if (!buffer)
			return NULL;

		BIO_read(outBIO, buffer, size);
	}

	BIO_free_all(outBIO);
	return buffer;
}