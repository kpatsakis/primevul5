void crypto_cert_free(CryptoCert cert)
{
	if (cert == NULL)
		return;

	X509_free(cert->px509);
	free(cert);
}