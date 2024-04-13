kssl_krb5_kt_get_entry(krb5_context context, krb5_keytab keytab,
                       krb5_const_principal principal, krb5_kvno vno,
                       krb5_enctype enctype, krb5_keytab_entry *entry)
	{
	if ( p_krb5_kt_get_entry )
		return(p_krb5_kt_get_entry(context,keytab,principal,vno,enctype,entry));
	else
		return KRB5KRB_ERR_GENERIC;
        }