krb5_error_code  kssl_check_authent(
			/* IN     */	KSSL_CTX	*kssl_ctx,
                        /* IN     */   	krb5_data	*authentp,
			/* OUT    */	krb5_timestamp	*atimep,
			/* OUT    */    KSSL_ERR	*kssl_err  )
	{
        krb5_error_code		krb5rc = 0;
	KRB5_ENCDATA		*dec_authent = NULL;
	KRB5_AUTHENTBODY	*auth = NULL;
	krb5_enctype		enctype;
	EVP_CIPHER_CTX		ciph_ctx;
	const EVP_CIPHER	*enc = NULL;
	unsigned char		iv[EVP_MAX_IV_LENGTH];
	const unsigned char	*p;
	unsigned char		*unenc_authent;
	int 			outl, unencbufsize;
	struct tm		tm_time, *tm_l, *tm_g;
	time_t			now, tl, tg, tr, tz_offset;

	EVP_CIPHER_CTX_init(&ciph_ctx);
	*atimep = 0;
	kssl_err_set(kssl_err, 0, "");

#ifndef KRB5CHECKAUTH
	authentp = NULL;
#else
#if	KRB5CHECKAUTH == 0
	authentp = NULL;
#endif
#endif	/* KRB5CHECKAUTH */

	if (authentp == NULL  ||  authentp->length == 0)  return 0;

#ifdef KSSL_DEBUG
        {
        unsigned int ui;
	printf("kssl_check_authent: authenticator[%d]:\n",authentp->length);
	p = authentp->data; 
	for (ui=0; ui < authentp->length; ui++)  printf("%02x ",p[ui]);
	printf("\n");
        }
#endif	/* KSSL_DEBUG */

	unencbufsize = 2 * authentp->length;
	if ((unenc_authent = calloc(1, unencbufsize)) == NULL)
		{
		kssl_err_set(kssl_err, SSL_R_KRB5_S_INIT,
			"Unable to allocate authenticator buffer.\n");
		krb5rc = KRB5KRB_ERR_GENERIC;
		goto err;
		}

	p = (unsigned char *)authentp->data;
	if ((dec_authent = d2i_KRB5_ENCDATA(NULL, &p,
					(long) authentp->length)) == NULL) 
		{
		kssl_err_set(kssl_err, SSL_R_KRB5_S_INIT,
                        "Error decoding authenticator.\n");
		krb5rc = KRB5KRB_AP_ERR_BAD_INTEGRITY;
		goto err;
		}

	enctype = dec_authent->etype->data[0];	/* should = kssl_ctx->enctype */
#if !defined(KRB5_MIT_OLD11)
            switch ( enctype ) {
            case ENCTYPE_DES3_CBC_SHA1:		/*    EVP_des_ede3_cbc();  */
            case ENCTYPE_DES3_CBC_SHA:
            case ENCTYPE_DES3_CBC_RAW:
                krb5rc = 0;                     /* Skip, can't handle derived keys */
                goto err;
            }
#endif
	enc = kssl_map_enc(enctype);
	memset(iv, 0, sizeof iv);       /* per RFC 1510 */

	if (enc == NULL)
		{
		/*  Disable kssl_check_authent for ENCTYPE_DES3_CBC_SHA1.
		**  This enctype indicates the authenticator was encrypted
		**  using key-usage derived keys which openssl cannot decrypt.
		*/
		goto err;
		}

        if (!EVP_CipherInit(&ciph_ctx,enc,kssl_ctx->key,iv,0))
                {
                kssl_err_set(kssl_err, SSL_R_KRB5_S_INIT,
                        "EVP_CipherInit error decrypting authenticator.\n");
                krb5rc = KRB5KRB_AP_ERR_BAD_INTEGRITY;
                goto err;
                }
        outl = dec_authent->cipher->length;
        if (!EVP_Cipher(&ciph_ctx,unenc_authent,dec_authent->cipher->data,outl))
                {
                kssl_err_set(kssl_err, SSL_R_KRB5_S_INIT,
                        "EVP_Cipher error decrypting authenticator.\n");
                krb5rc = KRB5KRB_AP_ERR_BAD_INTEGRITY;
                goto err;
                }
        EVP_CIPHER_CTX_cleanup(&ciph_ctx);

#ifdef KSSL_DEBUG
	printf("kssl_check_authent: decrypted authenticator[%d] =\n", outl);
	for (padl=0; padl < outl; padl++) printf("%02x ",unenc_authent[padl]);
	printf("\n");
#endif	/* KSSL_DEBUG */

	if ((p = kssl_skip_confound(enctype, unenc_authent)) == NULL)
		{
		kssl_err_set(kssl_err, SSL_R_KRB5_S_INIT,
                        "confounded by authenticator.\n");
		krb5rc = KRB5KRB_AP_ERR_BAD_INTEGRITY;
		goto err;
		}
	outl -= p - unenc_authent;

	if ((auth = (KRB5_AUTHENTBODY *) d2i_KRB5_AUTHENT(NULL, &p,
							  (long) outl))==NULL)
		{
		kssl_err_set(kssl_err, SSL_R_KRB5_S_INIT,
                        "Error decoding authenticator body.\n");
		krb5rc = KRB5KRB_AP_ERR_BAD_INTEGRITY;
		goto err;
		}

	memset(&tm_time,0,sizeof(struct tm));
	if (k_gmtime(auth->ctime, &tm_time)  &&
		((tr = mktime(&tm_time)) != (time_t)(-1)))
 		{
 		now  = time(&now);
 		tm_l = localtime(&now); 	tl = mktime(tm_l);
 		tm_g = gmtime(&now);		tg = mktime(tm_g);
 		tz_offset = tg - tl;

		*atimep = (krb5_timestamp)(tr - tz_offset);
 		}

#ifdef KSSL_DEBUG
	printf("kssl_check_authent: returns %d for client time ", *atimep);
	if (auth && auth->ctime && auth->ctime->length && auth->ctime->data)
		printf("%.*s\n", auth->ctime->length, auth->ctime->data);
	else	printf("NULL\n");
#endif	/* KSSL_DEBUG */

 err:
	if (auth)		KRB5_AUTHENT_free((KRB5_AUTHENT *) auth);
	if (dec_authent)	KRB5_ENCDATA_free(dec_authent);
	if (unenc_authent)	free(unenc_authent);
	EVP_CIPHER_CTX_cleanup(&ciph_ctx);
	return krb5rc;
	}