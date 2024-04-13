krb5_rc_default(krb5_context con, krb5_rcache *rc)
	{
	if (!krb5_loaded)
		load_krb5_dll();

	if ( p_krb5_rc_default )
		return(p_krb5_rc_default(con,rc));
	else
		return KRB5KRB_ERR_GENERIC;
	}