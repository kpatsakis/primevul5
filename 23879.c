valid_cksumtype(krb5_cksumtype ctype)
        {
        if (!krb5_loaded)
                load_krb5_dll();

        if ( p_valid_cksumtype )
                return(p_valid_cksumtype(ctype));
        else
                return KRB5KRB_ERR_GENERIC;
        }