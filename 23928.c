kssl_krb5_cc_default(krb5_context CO,
                    krb5_ccache  * pCC)
	{
	if (!krb5_loaded)
		load_krb5_dll();

	if ( p_krb5_cc_default )
		return(p_krb5_cc_default(CO,pCC));
	else
		return KRB5KRB_ERR_GENERIC;
	}