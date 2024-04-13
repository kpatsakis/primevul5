static int verify_cb(int ok, X509_STORE_CTX* csc)
{
	if (ok != 1)
	{
		int err = X509_STORE_CTX_get_error(csc);
		int derr = X509_STORE_CTX_get_error_depth(csc);
		X509* where = X509_STORE_CTX_get_current_cert(csc);
		const char* what = X509_verify_cert_error_string(err);
		char* name = crypto_cert_subject(where);

		WLog_WARN(TAG, "Certificate verification failure '%s (%d)' at stack position %d", what, err,
		          derr);
		WLog_WARN(TAG, "%s", name);

		free(name);
	}
	return ok;
}