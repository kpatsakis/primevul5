kssl_krb5_init_context(krb5_context * pCO)
	{
	if (!krb5_loaded)
		load_krb5_dll();

	if ( p_krb5_init_context )
		return(p_krb5_init_context(pCO));
	else
		return KRB5KRB_ERR_GENERIC;
	}