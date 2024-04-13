kssl_krb5_auth_con_free (krb5_context CO,
                        krb5_auth_context ACO)
	{
	if (!krb5_loaded)
		load_krb5_dll();

	if ( p_krb5_auth_con_free )
		return(p_krb5_auth_con_free(CO,ACO));
	else
		return KRB5KRB_ERR_GENERIC;
	}