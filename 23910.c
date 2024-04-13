kssl_krb5_free_context(krb5_context CO)
	{
	if (!krb5_loaded)
		load_krb5_dll();

	if ( p_krb5_free_context )
		p_krb5_free_context(CO);
	}