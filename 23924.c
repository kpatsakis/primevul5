kssl_krb5_cc_get_principal
    (krb5_context context, krb5_ccache cache,
      krb5_principal *principal)
	{
	if ( p_krb5_cc_get_principal )
		return(p_krb5_cc_get_principal(context,cache,principal));
	else
		return(krb5_x
			((cache)->ops->get_princ,(context, cache, principal)));
	}