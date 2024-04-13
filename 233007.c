void crypto_cert_print_info(X509* xcert)
{
	char* fp;
	char* issuer;
	char* subject;
	subject = crypto_cert_subject(xcert);
	issuer = crypto_cert_issuer(xcert);
	fp = crypto_cert_fingerprint(xcert);

	if (!fp)
	{
		WLog_ERR(TAG, "error computing fingerprint");
		goto out_free_issuer;
	}

	WLog_INFO(TAG, "Certificate details:");
	WLog_INFO(TAG, "\tSubject: %s", subject);
	WLog_INFO(TAG, "\tIssuer: %s", issuer);
	WLog_INFO(TAG, "\tThumbprint: %s", fp);
	WLog_INFO(TAG,
	          "The above X.509 certificate could not be verified, possibly because you do not have "
	          "the CA certificate in your certificate store, or the certificate has expired. "
	          "Please look at the OpenSSL documentation on how to add a private CA to the store.");
	free(fp);
out_free_issuer:
	free(issuer);
	free(subject);
}