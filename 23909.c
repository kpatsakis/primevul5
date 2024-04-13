kssl_krb5_get_server_rcache(krb5_context con, krb5_const krb5_data * data,
                            krb5_rcache * rcache) 
        {
	if ( p_krb5_get_server_rcache )
		return(p_krb5_get_server_rcache(con,data,rcache));
	else
		return KRB5KRB_ERR_GENERIC;
        }