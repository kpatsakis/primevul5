rdpCertificateData* crypto_get_certificate_data(X509* xcert, const char* hostname, UINT16 port)
{
	char* issuer;
	char* subject;
	char* fp;
	rdpCertificateData* certdata;
	fp = crypto_cert_fingerprint(xcert);

	if (!fp)
		return NULL;

	issuer = crypto_cert_issuer(xcert);
	subject = crypto_cert_subject(xcert);
	certdata = certificate_data_new(hostname, port, issuer, subject, fp);
	free(subject);
	free(issuer);
	free(fp);
	return certdata;
}