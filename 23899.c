krb5_principal_compare(krb5_context con, krb5_const_principal princ1,
                krb5_const_principal princ2)
	{
	if (!krb5_loaded)
		load_krb5_dll();

	if ( p_krb5_principal_compare )
		return(p_krb5_principal_compare(con,princ1,princ2));
	else
		return KRB5KRB_ERR_GENERIC;
	}