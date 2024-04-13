srs_hash_create_v(srs_t *srs, int idx, char *buf, int nargs, va_list ap)
{
#ifdef USE_OPENSSL
	HMAC_CTX		 ctx;
	int				 srshashlen;
	char			 srshash[EVP_MAX_MD_SIZE + 1];
#else
	srs_hmac_ctx_t	 ctx;
	char			 srshash[SHA_DIGESTSIZE + 1];
#endif
	char			*secret;
	char			*data;
	int				 len;
	char			*lcdata;
	unsigned char	*hp;
	char			*bp;
	int				 i;
	int				 j;

	secret = srs->secrets[idx];

#ifdef USE_OPENSSL
	HMAC_CTX_init(&ctx);
	HMAC_Init(&ctx, secret, strlen(secret), EVP_sha1());
#else
	srs_hmac_init(&ctx, secret, strlen(secret));
#endif

	for (i = 0; i < nargs; i++) {
		data = va_arg(ap, char *);
		len = strlen(data);
		lcdata = alloca(len + 1);
		for (j = 0; j < len; j++) {
			if (isupper(data[j]))
				lcdata[j] = tolower(data[j]);
			else
				lcdata[j] = data[j];
		}
#ifdef USE_OPENSSL
		HMAC_Update(&ctx, lcdata, len);
#else
		srs_hmac_update(&ctx, lcdata, len);
#endif
	}

#ifdef USE_OPENSSL
	HMAC_Final(&ctx, srshash, &srshashlen);
	HMAC_CTX_cleanup(&ctx);
	srshash[EVP_MAX_MD_SIZE] = '\0';
#else
	srs_hmac_fini(&ctx, srshash);
	srshash[SHA_DIGESTSIZE] = '\0';
#endif


	/* A little base64 encoding. Just a little. */
	hp = (unsigned char *)srshash;
	bp = buf;
	for (i = 0; i < srs->hashlength; i++) {
		switch (i & 0x03) {
			default:	/* NOTREACHED */
			case 0:
				j = (*hp >> 2);
				break;
			case 1:
				j = ((*hp & 0x03) << 4) |
						((*(hp + 1) & 0xF0) >> 4);
				hp++;
				break;
			case 2:
				j = ((*hp & 0x0F) << 2) |
						((*(hp + 1) & 0xC0) >> 6);
				hp++;
				break;
			case 3:
				j = (*hp++ & 0x3F);
				break;
		}
		*bp++ = SRS_HASH_BASECHARS[j];
	}

	*bp = '\0';
	buf[srs->hashlength] = '\0';
}