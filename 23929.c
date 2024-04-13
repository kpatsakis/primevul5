kssl_ctx_new(void)
        {
	return ((KSSL_CTX *) kssl_calloc(1, sizeof(KSSL_CTX)));
        }