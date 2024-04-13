kssl_krb5_auth_con_setrcache(krb5_context con, krb5_auth_context acon,
                             krb5_rcache rcache)
        {
        if ( p_krb5_auth_con_setrcache )
                 return(p_krb5_auth_con_setrcache(con,acon,rcache));
        else
                 return KRB5KRB_ERR_GENERIC;
        }