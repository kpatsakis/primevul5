krb5_rc_get_lifespan(krb5_context con, krb5_rcache rc, krb5_deltat *lifespanp)
	{
	if (!krb5_loaded)
		load_krb5_dll();

	if ( p_krb5_rc_get_lifespan )
		return(p_krb5_rc_get_lifespan(con, rc, lifespanp));
	else
		return KRB5KRB_ERR_GENERIC;
	}