kssl_krb5_free_principal(krb5_context c, krb5_principal p)
	{
	if (!krb5_loaded)
		load_krb5_dll();

	if ( p_krb5_free_principal )
		p_krb5_free_principal(c,p);
	}