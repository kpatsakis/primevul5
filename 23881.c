    kssl_keytab_is_available(KSSL_CTX *kssl_ctx)
{
    krb5_context		krb5context = NULL;
    krb5_keytab 		krb5keytab = NULL;
    krb5_keytab_entry           entry;
    krb5_principal              princ = NULL;
    krb5_error_code  		krb5rc = KRB5KRB_ERR_GENERIC;
    int rc = 0;

    if ((krb5rc = krb5_init_context(&krb5context)))
        return(0);

    /*	kssl_ctx->keytab_file == NULL ==> use Kerberos default
    */
    if (kssl_ctx->keytab_file)
    {
        krb5rc = krb5_kt_resolve(krb5context, kssl_ctx->keytab_file,
                                  &krb5keytab);
        if (krb5rc)
            goto exit;
    }
    else
    {
        krb5rc = krb5_kt_default(krb5context,&krb5keytab);
        if (krb5rc)
            goto exit;
    }

    /* the host key we are looking for */
    krb5rc = krb5_sname_to_principal(krb5context, NULL, 
                                     kssl_ctx->service_name ? kssl_ctx->service_name: KRB5SVC,
                                     KRB5_NT_SRV_HST, &princ);

    if (krb5rc)
	goto exit;

    krb5rc = krb5_kt_get_entry(krb5context, krb5keytab, 
                                princ,
                                0 /* IGNORE_VNO */,
                                0 /* IGNORE_ENCTYPE */,
                                &entry);
    if ( krb5rc == KRB5_KT_NOTFOUND ) {
        rc = 1;
        goto exit;
    } else if ( krb5rc )
        goto exit;
    
    krb5_kt_free_entry(krb5context, &entry);
    rc = 1;

  exit:
    if (krb5keytab)     krb5_kt_close(krb5context, krb5keytab);
    if (princ)          krb5_free_principal(krb5context, princ);
    if (krb5context)	krb5_free_context(krb5context);
    return(rc);
}