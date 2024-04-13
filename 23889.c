kssl_sget_tkt(	/* UPDATE */	KSSL_CTX		*kssl_ctx,
		/* IN     */	krb5_data		*indata,
		/* OUT    */	krb5_ticket_times	*ttimes,
		/* OUT    */	KSSL_ERR		*kssl_err  )
        {
        krb5_error_code			krb5rc = KRB5KRB_ERR_GENERIC;
        static krb5_context		krb5context = NULL;
	static krb5_auth_context	krb5auth_context = NULL;
	krb5_ticket 			*krb5ticket = NULL;
	KRB5_TKTBODY 			*asn1ticket = NULL;
	const unsigned char		*p;
	krb5_keytab 			krb5keytab = NULL;
	krb5_keytab_entry		kt_entry;
	krb5_principal			krb5server;
        krb5_rcache                     rcache = NULL;

	kssl_err_set(kssl_err, 0, "");

	if (!kssl_ctx)
                {
		kssl_err_set(kssl_err, SSL_R_KRB5_S_INIT,
			"No kssl_ctx defined.\n");
		goto err;
		}

#ifdef KSSL_DEBUG
	printf("in kssl_sget_tkt(%s)\n", kstring(kssl_ctx->service_name));
#endif	/* KSSL_DEBUG */

	if (!krb5context  &&  (krb5rc = krb5_init_context(&krb5context)))
                {
		kssl_err_set(kssl_err, SSL_R_KRB5_S_INIT,
                        "krb5_init_context() fails.\n");
		goto err;
		}
	if (krb5auth_context  &&
		(krb5rc = krb5_auth_con_free(krb5context, krb5auth_context)))
                {
		kssl_err_set(kssl_err, SSL_R_KRB5_S_INIT,
                        "krb5_auth_con_free() fails.\n");
		goto err;
		}
	else  krb5auth_context = NULL;
	if (!krb5auth_context  &&
		(krb5rc = krb5_auth_con_init(krb5context, &krb5auth_context)))
                {
		kssl_err_set(kssl_err, SSL_R_KRB5_S_INIT,
                        "krb5_auth_con_init() fails.\n");
		goto err;
		}

 
	if ((krb5rc = krb5_auth_con_getrcache(krb5context, krb5auth_context,
		&rcache)))
		{
 		kssl_err_set(kssl_err, SSL_R_KRB5_S_INIT,
			"krb5_auth_con_getrcache() fails.\n");
 		goto err;
		}
 
	if ((krb5rc = krb5_sname_to_principal(krb5context, NULL,
                (kssl_ctx->service_name)? kssl_ctx->service_name: KRB5SVC,
                KRB5_NT_SRV_HST, &krb5server)) != 0)
                {
		kssl_err_set(kssl_err, SSL_R_KRB5_S_INIT,
                        "krb5_sname_to_principal() fails.\n");
		goto err;
		}

	if (rcache == NULL) 
                {
                if ((krb5rc = krb5_get_server_rcache(krb5context,
			krb5_princ_component(krb5context, krb5server, 0),
			&rcache)))
                        {
		        kssl_err_set(kssl_err, SSL_R_KRB5_S_INIT,
                                "krb5_get_server_rcache() fails.\n");
                  	goto err;
                        }
                }

        if ((krb5rc = krb5_auth_con_setrcache(krb5context, krb5auth_context, rcache)))
                {
                kssl_err_set(kssl_err, SSL_R_KRB5_S_INIT,
			"krb5_auth_con_setrcache() fails.\n");
                goto err;
                }


	/*	kssl_ctx->keytab_file == NULL ==> use Kerberos default
	*/
	if (kssl_ctx->keytab_file)
		{
		krb5rc = krb5_kt_resolve(krb5context, kssl_ctx->keytab_file,
                        &krb5keytab);
		if (krb5rc)
			{
			kssl_err_set(kssl_err, SSL_R_KRB5_S_INIT,
				"krb5_kt_resolve() fails.\n");
			goto err;
			}
		}
	else
		{
                krb5rc = krb5_kt_default(krb5context,&krb5keytab);
                if (krb5rc)
			{
			kssl_err_set(kssl_err, SSL_R_KRB5_S_INIT, 
				"krb5_kt_default() fails.\n");
			goto err;
			}
		}

	/*	Actual Kerberos5 krb5_recvauth() has initial conversation here
	**	o	check KRB5_SENDAUTH_BADAUTHVERS
	**		unless KRB5_RECVAUTH_SKIP_VERSION
	**	o	check KRB5_SENDAUTH_BADAPPLVERS
	**	o	send "0" msg if all OK
	*/

	/*  20010411 was using AP_REQ instead of true KerberosWrapper
	**
	**  if ((krb5rc = krb5_rd_req(krb5context, &krb5auth_context,
	**			&krb5in_data, krb5server, krb5keytab,
	**			&ap_option, &krb5ticket)) != 0)  { Error }
	*/

	p = (unsigned char *)indata->data;
	if ((asn1ticket = (KRB5_TKTBODY *) d2i_KRB5_TICKET(NULL, &p,
						(long) indata->length)) == NULL)
		{
		BIO_snprintf(kssl_err->text, KSSL_ERR_MAX,
			"d2i_KRB5_TICKET() ASN.1 decode failure.\n");
		kssl_err->reason = SSL_R_KRB5_S_RD_REQ;
		goto err;
		}
	
	/* Was:  krb5rc = krb5_decode_ticket(krb5in_data,&krb5ticket)) != 0) */
	if ((krb5rc = kssl_TKT2tkt(krb5context, asn1ticket, &krb5ticket,
					kssl_err)) != 0)
		{
		BIO_snprintf(kssl_err->text, KSSL_ERR_MAX,
			"Error converting ASN.1 ticket to krb5_ticket.\n");
		kssl_err->reason = SSL_R_KRB5_S_RD_REQ;
		goto err;
		}

	if (! krb5_principal_compare(krb5context, krb5server,
						  krb5ticket->server))  {
		krb5rc = KRB5_PRINC_NOMATCH;
		BIO_snprintf(kssl_err->text, KSSL_ERR_MAX,
			"server principal != ticket principal\n");
		kssl_err->reason = SSL_R_KRB5_S_RD_REQ;
		goto err;
		}
	if ((krb5rc = krb5_kt_get_entry(krb5context, krb5keytab,
			krb5ticket->server, krb5ticket->enc_part.kvno,
			krb5ticket->enc_part.enctype, &kt_entry)) != 0)  {
		BIO_snprintf(kssl_err->text, KSSL_ERR_MAX,
			"krb5_kt_get_entry() fails with %x.\n", krb5rc);
		kssl_err->reason = SSL_R_KRB5_S_RD_REQ;
		goto err;
		}
	if ((krb5rc = krb5_decrypt_tkt_part(krb5context, &kt_entry.key,
			krb5ticket)) != 0)  {
		BIO_snprintf(kssl_err->text, KSSL_ERR_MAX,
			"krb5_decrypt_tkt_part() failed.\n");
		kssl_err->reason = SSL_R_KRB5_S_RD_REQ;
		goto err;
		}
	else  {
		krb5_kt_free_entry(krb5context, &kt_entry);
#ifdef KSSL_DEBUG
		{
		int i; krb5_address **paddr = krb5ticket->enc_part2->caddrs;
		printf("Decrypted ticket fields:\n");
		printf("\tflags: %X, transit-type: %X",
			krb5ticket->enc_part2->flags,
			krb5ticket->enc_part2->transited.tr_type);
		print_krb5_data("\ttransit-data: ",
			&(krb5ticket->enc_part2->transited.tr_contents));
		printf("\tcaddrs: %p, authdata: %p\n",
			krb5ticket->enc_part2->caddrs,
			krb5ticket->enc_part2->authorization_data);
		if (paddr)
			{
			printf("\tcaddrs:\n");
			for (i=0; paddr[i] != NULL; i++)
				{
				krb5_data d;
				d.length=paddr[i]->length;
				d.data=paddr[i]->contents;
				print_krb5_data("\t\tIP: ", &d);
				}
			}
		printf("\tstart/auth/end times: %d / %d / %d\n",
			krb5ticket->enc_part2->times.starttime,
			krb5ticket->enc_part2->times.authtime,
			krb5ticket->enc_part2->times.endtime);
		}
#endif	/* KSSL_DEBUG */
		}

	krb5rc = KRB5_NO_TKT_SUPPLIED;
	if (!krb5ticket  ||	!krb5ticket->enc_part2  ||
                !krb5ticket->enc_part2->client  ||
                !krb5ticket->enc_part2->client->data  ||
                !krb5ticket->enc_part2->session)
                {
                kssl_err_set(kssl_err, SSL_R_KRB5_S_BAD_TICKET,
                        "bad ticket from krb5_rd_req.\n");
		}
	else if (kssl_ctx_setprinc(kssl_ctx, KSSL_CLIENT,
		 &krb5ticket->enc_part2->client->realm,
		 krb5ticket->enc_part2->client->data,
		 krb5ticket->enc_part2->client->length))
                {
		kssl_err_set(kssl_err, SSL_R_KRB5_S_BAD_TICKET,
                        "kssl_ctx_setprinc() fails.\n");
		}
	else if (kssl_ctx_setkey(kssl_ctx, krb5ticket->enc_part2->session))
                {
		kssl_err_set(kssl_err, SSL_R_KRB5_S_BAD_TICKET,
                        "kssl_ctx_setkey() fails.\n");
		}
	else if (krb5ticket->enc_part2->flags & TKT_FLG_INVALID)
                {
		krb5rc = KRB5KRB_AP_ERR_TKT_INVALID;
                kssl_err_set(kssl_err, SSL_R_KRB5_S_BAD_TICKET,
                        "invalid ticket from krb5_rd_req.\n");
		}
	else	krb5rc = 0;

	kssl_ctx->enctype	= krb5ticket->enc_part.enctype;
	ttimes->authtime	= krb5ticket->enc_part2->times.authtime;
	ttimes->starttime	= krb5ticket->enc_part2->times.starttime;
	ttimes->endtime 	= krb5ticket->enc_part2->times.endtime;
	ttimes->renew_till	= krb5ticket->enc_part2->times.renew_till;

 err:
#ifdef KSSL_DEBUG
	kssl_ctx_show(kssl_ctx);
#endif	/* KSSL_DEBUG */

	if (asn1ticket) 	KRB5_TICKET_free((KRB5_TICKET *) asn1ticket);
        if (krb5keytab)         krb5_kt_close(krb5context, krb5keytab);
	if (krb5ticket) 	krb5_free_ticket(krb5context, krb5ticket);
	if (krb5server) 	krb5_free_principal(krb5context, krb5server);
	return (krb5rc);
        }