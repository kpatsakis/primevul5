kssl_ctx_setkey(KSSL_CTX *kssl_ctx, krb5_keyblock *session)
        {
	int 		length;
	krb5_enctype	enctype;
	krb5_octet FAR	*contents = NULL;

	if (!kssl_ctx)  return KSSL_CTX_ERR;

	if (kssl_ctx->key)
                {
		OPENSSL_cleanse(kssl_ctx->key, kssl_ctx->length);
		kssl_free(kssl_ctx->key);
		}

	if (session)
                {

#ifdef KRB5_HEIMDAL
		length = session->keyvalue->length;
		enctype = session->keytype;
		contents = session->keyvalue->contents;
#else
		length = session->length;
		enctype = session->enctype;
		contents = session->contents;
#endif
		kssl_ctx->enctype = enctype;
		kssl_ctx->length  = length;
		}
	else
                {
		kssl_ctx->enctype = ENCTYPE_UNKNOWN;
		kssl_ctx->length  = 0;
		return KSSL_CTX_OK;
		}

	if ((kssl_ctx->key =
                (krb5_octet FAR *) kssl_calloc(1, kssl_ctx->length)) == NULL)
                {
		kssl_ctx->length  = 0;
		return KSSL_CTX_ERR;
		}
	else
		memcpy(kssl_ctx->key, contents, length);

	return KSSL_CTX_OK;
        }