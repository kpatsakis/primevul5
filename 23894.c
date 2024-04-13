krb5_error_code  kssl_build_principal_2(
			/* UPDATE */	krb5_context	context,
			/* OUT    */	krb5_principal	*princ,
			/* IN     */	int rlen,  const char *realm,
			/* IN	  */	int slen,  const char *svc,
			/* IN	  */	int hlen,  const char *host)
	{
	krb5_data		*p_data = NULL;
	krb5_principal		new_p = NULL;
        char			*new_r = NULL;

	if ((p_data = (krb5_data *) calloc(2, sizeof(krb5_data))) == NULL  ||
	    (new_p = (krb5_principal) calloc(1, sizeof(krb5_principal_data)))
			== NULL)  goto err;
	new_p->length = 2;
	new_p->data = p_data;

	if ((new_r = calloc(1, rlen + 1)) == NULL)  goto err;
	memcpy(new_r, realm, rlen);
	krb5_princ_set_realm_length(context, new_p, rlen);
	krb5_princ_set_realm_data(context, new_p, new_r);

	if ((new_p->data[0].data = calloc(1, slen + 1)) == NULL)  goto err;
	memcpy(new_p->data[0].data, svc, slen);
	new_p->data[0].length = slen;

	if ((new_p->data[1].data = calloc(1, hlen + 1)) == NULL)  goto err;
	memcpy(new_p->data[1].data, host, hlen);
	new_p->data[1].length = hlen;
	
	krb5_princ_type(context, new_p) = KRB5_NT_UNKNOWN;
	*princ = new_p;
	return 0;

 err:
	if (new_p  &&  new_p[0].data)	free(new_p[0].data);
	if (new_p  &&  new_p[1].data)	free(new_p[1].data);
	if (new_p)	free(new_p);
	if (new_r)	free(new_r);
	return ENOMEM;
	}