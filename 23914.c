kssl_err_set(KSSL_ERR *kssl_err, int reason, char *text)
        {
	if (kssl_err == NULL)  return;

	kssl_err->reason = reason;
	BIO_snprintf(kssl_err->text, KSSL_ERR_MAX, "%s", text);
	return;
        }