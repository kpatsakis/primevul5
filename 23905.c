kssl_krb5_get_credentials(krb5_context CO,
                         krb5_const krb5_flags F,
                         krb5_ccache CC,
                         krb5_creds  * pCR,
                         krb5_creds  ** ppCR)
	{
	if (!krb5_loaded)
		load_krb5_dll();

	if ( p_krb5_get_credentials )
		return(p_krb5_get_credentials(CO,F,CC,pCR,ppCR));
	else
		return KRB5KRB_ERR_GENERIC;
	}