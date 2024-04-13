krb5_rc_initialize(krb5_context con, krb5_rcache rc, krb5_deltat lifespan)
	{
	if (!krb5_loaded)
		load_krb5_dll();

	if ( p_krb5_rc_initialize )
		return(p_krb5_rc_initialize(con, rc, lifespan));
	else
		return KRB5KRB_ERR_GENERIC;
	}