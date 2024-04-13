kssl_ctx_free(KSSL_CTX *kssl_ctx)
        {
	if (kssl_ctx == NULL)  return kssl_ctx;

	if (kssl_ctx->key)  		OPENSSL_cleanse(kssl_ctx->key,
							      kssl_ctx->length);
	if (kssl_ctx->key)  		kssl_free(kssl_ctx->key);
	if (kssl_ctx->client_princ) 	kssl_free(kssl_ctx->client_princ);
	if (kssl_ctx->service_host) 	kssl_free(kssl_ctx->service_host);
	if (kssl_ctx->service_name) 	kssl_free(kssl_ctx->service_name);
	if (kssl_ctx->keytab_file) 	kssl_free(kssl_ctx->keytab_file);

	kssl_free(kssl_ctx);
	return (KSSL_CTX *) NULL;
        }