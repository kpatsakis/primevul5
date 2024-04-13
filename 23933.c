kssl_krb5_kt_resolve(krb5_context con,
                    krb5_const char * sz,
                    krb5_keytab * kt)
	{
	if (!krb5_loaded)
		load_krb5_dll();

	if ( p_krb5_kt_resolve )
		return(p_krb5_kt_resolve(con,sz,kt));
	else
		return KRB5KRB_ERR_GENERIC;
	}