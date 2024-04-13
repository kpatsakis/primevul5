CryptoCert crypto_cert_read(BYTE* data, UINT32 length)
{
	CryptoCert cert = malloc(sizeof(*cert));

	if (!cert)
		return NULL;

	/* this will move the data pointer but we don't care, we don't use it again */
	cert->px509 = d2i_X509(NULL, (D2I_X509_CONST BYTE**)&data, length);
	return cert;
}