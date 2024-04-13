char* crypto_cert_fingerprint(X509* xcert)
{
	return crypto_cert_fingerprint_by_hash(xcert, "sha256");
}