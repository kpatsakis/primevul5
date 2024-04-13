kssl_krb5_free_ticket(krb5_context con,
                     krb5_ticket * kt)
	{
	if (!krb5_loaded)
		load_krb5_dll();

	if ( p_krb5_free_ticket )
		return(p_krb5_free_ticket(con,kt));
	else
		return KRB5KRB_ERR_GENERIC;
	}