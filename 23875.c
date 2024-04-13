krb5_kt_free_entry(krb5_context con,krb5_keytab_entry * entry)
        {
        if (!krb5_loaded)
                load_krb5_dll();

        if ( p_krb5_kt_free_entry )
                return(p_krb5_kt_free_entry(con,entry));
        else
                return KRB5KRB_ERR_GENERIC;
        }