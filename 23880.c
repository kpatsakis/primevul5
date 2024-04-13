kssl_krb5_auth_con_getrcache(krb5_context con, krb5_auth_context acon,
                             krb5_rcache * prcache)
        {
	if ( p_krb5_auth_con_getrcache )
		return(p_krb5_auth_con_getrcache(con,acon, prcache));
	else
		return KRB5KRB_ERR_GENERIC;
	}