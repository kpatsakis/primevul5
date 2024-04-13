krb5_decrypt_tkt_part(krb5_context con, krb5_const krb5_keyblock *keys,
                krb5_ticket *ticket)
	{
	if (!krb5_loaded)
		load_krb5_dll();

	if ( p_krb5_decrypt_tkt_part )
		return(p_krb5_decrypt_tkt_part(con,keys,ticket));
	else
		return KRB5KRB_ERR_GENERIC;
	}