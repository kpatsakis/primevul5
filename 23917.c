kssl_krb5_kt_default(krb5_context con,
                    krb5_keytab * kt)
	{
	if (!krb5_loaded)
		load_krb5_dll();

	if ( p_krb5_kt_default )
		return(p_krb5_kt_default(con,kt));
	else
		return KRB5KRB_ERR_GENERIC;
	}