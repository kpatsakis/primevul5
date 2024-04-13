kssl_krb5_rd_req(krb5_context con, krb5_auth_context * pacon,
                krb5_const krb5_data * data,
                krb5_const_principal princ, krb5_keytab keytab,
                krb5_flags * flags, krb5_ticket ** pptkt)
	{
	if (!krb5_loaded)
		load_krb5_dll();

	if ( p_krb5_rd_req )
		return(p_krb5_rd_req(con,pacon,data,princ,keytab,flags,pptkt));
	else
		return KRB5KRB_ERR_GENERIC;
	}