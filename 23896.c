kssl_ctx_show(KSSL_CTX *kssl_ctx)
        {
	int 	i;

	printf("kssl_ctx: ");
	if (kssl_ctx == NULL)
                {
		printf("NULL\n");
		return;
		}
	else
		printf("%p\n", (void *)kssl_ctx);

	printf("\tservice:\t%s\n",
                (kssl_ctx->service_name)? kssl_ctx->service_name: "NULL");
	printf("\tclient:\t%s\n",
                (kssl_ctx->client_princ)? kssl_ctx->client_princ: "NULL");
	printf("\tserver:\t%s\n",
                (kssl_ctx->service_host)? kssl_ctx->service_host: "NULL");
	printf("\tkeytab:\t%s\n",
                (kssl_ctx->keytab_file)? kssl_ctx->keytab_file: "NULL");
	printf("\tkey [%d:%d]:\t",
                kssl_ctx->enctype, kssl_ctx->length);

	for (i=0; i < kssl_ctx->length  &&  kssl_ctx->key; i++)
                {
		printf("%02x", kssl_ctx->key[i]);
		}
	printf("\n");
	return;
        }