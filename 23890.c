kssl_tgt_is_available(KSSL_CTX *kssl_ctx)
        {
        krb5_error_code		krb5rc = KRB5KRB_ERR_GENERIC;
        krb5_context		krb5context = NULL;
        krb5_ccache 		krb5ccdef = NULL;
        krb5_creds		krb5creds, *krb5credsp = NULL;
        int                     rc = 0;

        memset((char *)&krb5creds, 0, sizeof(krb5creds));

        if (!kssl_ctx)
            return(0);

        if (!kssl_ctx->service_host)
            return(0);

        if ((krb5rc = krb5_init_context(&krb5context)) != 0)
            goto err;

        if ((krb5rc = krb5_sname_to_principal(krb5context,
                                              kssl_ctx->service_host,
                                              (kssl_ctx->service_name)? kssl_ctx->service_name: KRB5SVC,
                                              KRB5_NT_SRV_HST, &krb5creds.server)) != 0)
            goto err;

        if ((krb5rc = krb5_cc_default(krb5context, &krb5ccdef)) != 0)
            goto err;

        if ((krb5rc = krb5_cc_get_principal(krb5context, krb5ccdef,
                                             &krb5creds.client)) != 0)
            goto err;

        if ((krb5rc = krb5_get_credentials(krb5context, 0, krb5ccdef,
                                            &krb5creds, &krb5credsp)) != 0)
            goto err;

        rc = 1;

      err:
#ifdef KSSL_DEBUG
	kssl_ctx_show(kssl_ctx);
#endif	/* KSSL_DEBUG */

	if (krb5creds.client)	krb5_free_principal(krb5context, krb5creds.client);
	if (krb5creds.server)	krb5_free_principal(krb5context, krb5creds.server);
	if (krb5context)	krb5_free_context(krb5context);
        return(rc);
	}