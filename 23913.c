kssl_cget_tkt(	/* UPDATE */	KSSL_CTX *kssl_ctx,
                /* OUT    */	krb5_data **enc_ticketp,
                /* UPDATE */	krb5_data *authenp,
                /* OUT    */	KSSL_ERR *kssl_err)
	{
	krb5_error_code		krb5rc = KRB5KRB_ERR_GENERIC;
	krb5_context		krb5context = NULL;
	krb5_auth_context	krb5auth_context = NULL;
	krb5_ccache 		krb5ccdef = NULL;
	krb5_creds		krb5creds, *krb5credsp = NULL;
	krb5_data		krb5_app_req;

	kssl_err_set(kssl_err, 0, "");
	memset((char *)&krb5creds, 0, sizeof(krb5creds));

	if (!kssl_ctx)
                {
		kssl_err_set(kssl_err, SSL_R_KRB5_S_INIT,
                        "No kssl_ctx defined.\n");
		goto err;
		}
	else if (!kssl_ctx->service_host)
                {
		kssl_err_set(kssl_err, SSL_R_KRB5_S_INIT,
                        "kssl_ctx service_host undefined.\n");
		goto err;
		}

	if ((krb5rc = krb5_init_context(&krb5context)) != 0)
                {
		BIO_snprintf(kssl_err->text,KSSL_ERR_MAX,
                        "krb5_init_context() fails: %d\n", krb5rc);
		kssl_err->reason = SSL_R_KRB5_C_INIT;
		goto err;
		}

	if ((krb5rc = krb5_sname_to_principal(krb5context,
                kssl_ctx->service_host,
                (kssl_ctx->service_name)? kssl_ctx->service_name: KRB5SVC,
                KRB5_NT_SRV_HST, &krb5creds.server)) != 0)
                {
		BIO_snprintf(kssl_err->text,KSSL_ERR_MAX,
                        "krb5_sname_to_principal() fails for %s/%s\n",
                        kssl_ctx->service_host,
                        (kssl_ctx->service_name)? kssl_ctx->service_name:
						  KRB5SVC);
		kssl_err->reason = SSL_R_KRB5_C_INIT;
		goto err;
		}

	if ((krb5rc = krb5_cc_default(krb5context, &krb5ccdef)) != 0)
                {
		kssl_err_set(kssl_err, SSL_R_KRB5_C_CC_PRINC,
                        "krb5_cc_default fails.\n");
		goto err;
		}

	if ((krb5rc = krb5_cc_get_principal(krb5context, krb5ccdef,
                &krb5creds.client)) != 0)
                {
		kssl_err_set(kssl_err, SSL_R_KRB5_C_CC_PRINC,
                        "krb5_cc_get_principal() fails.\n");
		goto err;
		}

	if ((krb5rc = krb5_get_credentials(krb5context, 0, krb5ccdef,
                &krb5creds, &krb5credsp)) != 0)
                {
		kssl_err_set(kssl_err, SSL_R_KRB5_C_GET_CRED,
                        "krb5_get_credentials() fails.\n");
		goto err;
		}

	*enc_ticketp = &krb5credsp->ticket;
#ifdef KRB5_HEIMDAL
	kssl_ctx->enctype = krb5credsp->session.keytype;
#else
	kssl_ctx->enctype = krb5credsp->keyblock.enctype;
#endif

	krb5rc = KRB5KRB_ERR_GENERIC;
	/*	caller should free data of krb5_app_req  */
	/*  20010406 VRS deleted for real KerberosWrapper
	**  20010605 VRS reinstated to offer Authenticator to KerberosWrapper
	*/
	krb5_app_req.length = 0;
	if (authenp)
                {
		krb5_data	krb5in_data;
		const unsigned char	*p;
		long		arlen;
		KRB5_APREQBODY	*ap_req;

		authenp->length = 0;
		krb5in_data.data = NULL;
		krb5in_data.length = 0;
		if ((krb5rc = krb5_mk_req_extended(krb5context,
			&krb5auth_context, 0, &krb5in_data, krb5credsp,
			&krb5_app_req)) != 0)
			{
			kssl_err_set(kssl_err, SSL_R_KRB5_C_MK_REQ,
				"krb5_mk_req_extended() fails.\n");
			goto err;
			}

		arlen = krb5_app_req.length;
		p = (unsigned char *)krb5_app_req.data;
		ap_req = (KRB5_APREQBODY *) d2i_KRB5_APREQ(NULL, &p, arlen);
		if (ap_req)
			{
			authenp->length = i2d_KRB5_ENCDATA(
					ap_req->authenticator, NULL);
			if (authenp->length  && 
				(authenp->data = malloc(authenp->length)))
				{
				unsigned char	*adp = (unsigned char *)authenp->data;
				authenp->length = i2d_KRB5_ENCDATA(
						ap_req->authenticator, &adp);
				}
			}

		if (ap_req)  KRB5_APREQ_free((KRB5_APREQ *) ap_req);
		if (krb5_app_req.length)  
                        kssl_krb5_free_data_contents(krb5context,&krb5_app_req);
		}
#ifdef KRB5_HEIMDAL
	if (kssl_ctx_setkey(kssl_ctx, &krb5credsp->session))
                {
		kssl_err_set(kssl_err, SSL_R_KRB5_C_INIT,
                        "kssl_ctx_setkey() fails.\n");
		}
#else
	if (kssl_ctx_setkey(kssl_ctx, &krb5credsp->keyblock))
                {
		kssl_err_set(kssl_err, SSL_R_KRB5_C_INIT,
                        "kssl_ctx_setkey() fails.\n");
		}
#endif
	else	krb5rc = 0;

 err:
#ifdef KSSL_DEBUG
	kssl_ctx_show(kssl_ctx);
#endif	/* KSSL_DEBUG */

	if (krb5creds.client)	krb5_free_principal(krb5context,
							krb5creds.client);
	if (krb5creds.server)	krb5_free_principal(krb5context,
							krb5creds.server);
	if (krb5auth_context)	krb5_auth_con_free(krb5context,
							krb5auth_context);
	if (krb5context)	krb5_free_context(krb5context);
	return (krb5rc);
	}