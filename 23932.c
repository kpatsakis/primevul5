kssl_krb5_auth_con_init(krb5_context CO,
                       krb5_auth_context  * pACO)
	{
	if (!krb5_loaded)
		load_krb5_dll();

	if ( p_krb5_auth_con_init )
		return(p_krb5_auth_con_init(CO,pACO));
	else
		return KRB5KRB_ERR_GENERIC;
	}