krb5_checksum_size(krb5_context context,krb5_cksumtype ctype)
        {
        if (!krb5_loaded)
                load_krb5_dll();

        if ( p_krb5_checksum_size )
                return(p_krb5_checksum_size(context, ctype));
        else
                return KRB5KRB_ERR_GENERIC;
        }