kssl_krb5_mk_req_extended (krb5_context CO,
                          krb5_auth_context  * pACO,
                          krb5_const krb5_flags F,
                          krb5_data  * pD1,
                          krb5_creds  * pC,
                          krb5_data  * pD2)
	{
	if (!krb5_loaded)
		load_krb5_dll();

	if ( p_krb5_mk_req_extended )
		return(p_krb5_mk_req_extended(CO,pACO,F,pD1,pC,pD2));
	else
		return KRB5KRB_ERR_GENERIC;
	}