kssl_krb5_kt_close(krb5_context context, krb5_keytab keytab)
	{
	if ( p_krb5_kt_close )
		return(p_krb5_kt_close(context,keytab));
	else 
		return KRB5KRB_ERR_GENERIC;
	}