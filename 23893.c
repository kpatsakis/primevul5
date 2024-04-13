kssl_TKT2tkt(	/* IN     */	krb5_context	krb5context,
		/* IN     */	KRB5_TKTBODY	*asn1ticket,
		/* OUT    */	krb5_ticket	**krb5ticket,
		/* OUT    */	KSSL_ERR *kssl_err  )
        {
        krb5_error_code			krb5rc = KRB5KRB_ERR_GENERIC;
	krb5_ticket 			*new5ticket = NULL;
	ASN1_GENERALSTRING		*gstr_svc, *gstr_host;

	*krb5ticket = NULL;

	if (asn1ticket == NULL  ||  asn1ticket->realm == NULL  ||
		asn1ticket->sname == NULL  || 
		sk_ASN1_GENERALSTRING_num(asn1ticket->sname->namestring) < 2)
		{
		BIO_snprintf(kssl_err->text, KSSL_ERR_MAX,
			"Null field in asn1ticket.\n");
		kssl_err->reason = SSL_R_KRB5_S_RD_REQ;
		return KRB5KRB_ERR_GENERIC;
		}

	if ((new5ticket = (krb5_ticket *) calloc(1, sizeof(krb5_ticket)))==NULL)
		{
		BIO_snprintf(kssl_err->text, KSSL_ERR_MAX,
			"Unable to allocate new krb5_ticket.\n");
		kssl_err->reason = SSL_R_KRB5_S_RD_REQ;
		return ENOMEM;		/*  or  KRB5KRB_ERR_GENERIC;	*/
		}

	gstr_svc  = sk_ASN1_GENERALSTRING_value(asn1ticket->sname->namestring, 0);
	gstr_host = sk_ASN1_GENERALSTRING_value(asn1ticket->sname->namestring, 1);

	if ((krb5rc = kssl_build_principal_2(krb5context,
			&new5ticket->server,
			asn1ticket->realm->length, (char *)asn1ticket->realm->data,
			gstr_svc->length,  (char *)gstr_svc->data,
			gstr_host->length, (char *)gstr_host->data)) != 0)
		{
		free(new5ticket);
		BIO_snprintf(kssl_err->text, KSSL_ERR_MAX,
			"Error building ticket server principal.\n");
		kssl_err->reason = SSL_R_KRB5_S_RD_REQ;
		return krb5rc;		/*  or  KRB5KRB_ERR_GENERIC;	*/
		}

	krb5_princ_type(krb5context, new5ticket->server) =
			asn1ticket->sname->nametype->data[0];
	new5ticket->enc_part.enctype = asn1ticket->encdata->etype->data[0];
	new5ticket->enc_part.kvno = asn1ticket->encdata->kvno->data[0];
	new5ticket->enc_part.ciphertext.length =
			asn1ticket->encdata->cipher->length;
	if ((new5ticket->enc_part.ciphertext.data =
		calloc(1, asn1ticket->encdata->cipher->length)) == NULL)
		{
		free(new5ticket);
		BIO_snprintf(kssl_err->text, KSSL_ERR_MAX,
			"Error allocating cipher in krb5ticket.\n");
		kssl_err->reason = SSL_R_KRB5_S_RD_REQ;
		return KRB5KRB_ERR_GENERIC;
		}
	else
		{
		memcpy(new5ticket->enc_part.ciphertext.data,
			asn1ticket->encdata->cipher->data,
			asn1ticket->encdata->cipher->length);
		}

	*krb5ticket = new5ticket;
	return 0;
	}