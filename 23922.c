kssl_krb5_sname_to_principal(krb5_context CO,
                            krb5_const char  * pC1,
                            krb5_const char  * pC2,
                            krb5_int32 I,
                            krb5_principal  * pPR)
	{
	if (!krb5_loaded)
		load_krb5_dll();

	if ( p_krb5_sname_to_principal )
		return(p_krb5_sname_to_principal(CO,pC1,pC2,I,pPR));
	else
		return KRB5KRB_ERR_GENERIC;
	}