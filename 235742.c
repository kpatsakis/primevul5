ngx_http_auth_spnego_basic(
        ngx_http_request_t * r,
        ngx_http_auth_spnego_ctx_t * ctx,
        ngx_http_auth_spnego_loc_conf_t * alcf)
{
    ngx_str_t host_name;
    ngx_str_t service;
    ngx_str_t user;
    user.data = NULL;
    ngx_str_t new_user;
    ngx_int_t ret = NGX_DECLINED;

    krb5_context kcontext = NULL;
    krb5_error_code code;
    krb5_principal client = NULL;
    krb5_principal server = NULL;
    krb5_creds creds;
    krb5_get_init_creds_opt *gic_options = NULL;
    int kret = 0;
    char *name = NULL;
    char *p = NULL;

    code = krb5_init_context(&kcontext);
    if (code) {
        spnego_debug0("Kerberos error: Cannot initialize kerberos context");
        return NGX_ERROR;
    }

    host_name = r->headers_in.host->value;
    service.len = alcf->srvcname.len + alcf->realm.len + 3;

    if (ngx_strchr(alcf->srvcname.data, '/')) {
        service.data = ngx_palloc(r->pool, service.len);
        if (NULL == service.data) {
            spnego_error(NGX_ERROR);
        }

        ngx_snprintf(service.data, service.len, "%V@%V%Z",
                &alcf->srvcname, &alcf->realm);
    } else {
        service.len += host_name.len;
        service.data = ngx_palloc(r->pool, service.len);
        if (NULL == service.data) {
            spnego_error(NGX_ERROR);
        }

        ngx_snprintf(service.data, service.len, "%V/%V@%V%Z",
                &alcf->srvcname, &host_name, &alcf->realm);
    }

    kret = krb5_parse_name(kcontext, (const char *) service.data, &server);

    if (kret) {
        spnego_log_error("Kerberos error:  Unable to parse service name");
        spnego_log_krb5_error(kcontext, code);
        spnego_error(NGX_ERROR);
    }

    code = krb5_unparse_name(kcontext, server, &name);
    if (code) {
        spnego_log_error("Kerberos error: Cannot unparse servicename");
        spnego_log_krb5_error(kcontext, code);
        spnego_error(NGX_ERROR);
    }

    free(name);
    name = NULL;

    p = ngx_strchr(r->headers_in.user.data, '@');
    user.len = r->headers_in.user.len + 1;
    if (NULL == p) {
        if (alcf->force_realm && alcf->realm.len && alcf->realm.data ) {
            user.len += alcf->realm.len + 1; /* +1 for @ */
            user.data = ngx_palloc(r->pool, user.len);
            if (NULL == user.data) {
                spnego_log_error("Not enough memory");
                spnego_error(NGX_ERROR);
            }
            ngx_snprintf(user.data, user.len, "%V@%V%Z", &r->headers_in.user,
                    &alcf->realm);
        } else {
            user.data = ngx_palloc(r->pool, user.len);
            if (NULL == user.data) {
                spnego_log_error("Not enough memory");
                spnego_error(NGX_ERROR);
            }
            ngx_snprintf(user.data, user.len, "%V%Z", &r->headers_in.user);
        }
    } else {
        if (alcf->realm.len && alcf->realm.data && ngx_strncmp(p + 1,
                    alcf->realm.data, alcf->realm.len) == 0) {
            user.data = ngx_palloc(r->pool, user.len);
            if (NULL == user.data) {
                spnego_log_error("Not enough memory");
                spnego_error(NGX_ERROR);
            }
            ngx_snprintf(user.data, user.len, "%V%Z",
                    &r->headers_in.user);
            if (alcf->fqun == 0) {
                /*
                 * Specified realm is identical to configured realm.
                 * Truncate $remote_user to strip @REALM.
                 */
                r->headers_in.user.len -= alcf->realm.len + 1;
            }
        } else if (alcf->force_realm) {
            *p = '\0';
            user.len = ngx_strlen(r->headers_in.user.data) + 1;
            if (alcf->realm.len && alcf->realm.data)
                user.len += alcf->realm.len + 1;
            user.data = ngx_pcalloc(r->pool, user.len);
            if (NULL == user.data) {
                spnego_log_error("Not enough memory");
                spnego_error(NGX_ERROR);
            }
            if (alcf->realm.len && alcf->realm.data)
                ngx_snprintf(user.data, user.len, "%s@%V%Z",
                        r->headers_in.user.data, &alcf->realm);
            else
                ngx_snprintf(user.data, user.len, "%s%Z",
                        r->headers_in.user.data);
            /*
             * Rewrite $remote_user with the forced realm.
             * If the forced realm is shorter than the
             * specified realm, we can reuse the original
             * buffer.
             */
            if (r->headers_in.user.len >= user.len - 1)
                r->headers_in.user.len = user.len - 1;
            else {
                new_user.len = user.len - 1;
                new_user.data = ngx_palloc(r->pool, new_user.len);
                if (NULL == new_user.data) {
                    spnego_log_error("Not enough memory");
                    spnego_error(NGX_ERROR);
                }
                ngx_pfree(r->pool, r->headers_in.user.data);
                r->headers_in.user.data = new_user.data;
                r->headers_in.user.len = new_user.len;
            }
            ngx_memcpy(r->headers_in.user.data, user.data,
                    r->headers_in.user.len);
        } else {
            user.data = ngx_palloc(r->pool, user.len);
            if (NULL == user.data) {
                spnego_log_error("Not enough memory");
                spnego_error(NGX_ERROR);
            }
            ngx_snprintf(user.data, user.len, "%V%Z", &r->headers_in.user);
        }
    }

    spnego_debug1("Attempting authentication with principal %s",
            (const char *)user.data);

    code = krb5_parse_name(kcontext, (const char *) user.data, &client);
    if (code) {
        spnego_log_error("Kerberos error: Unable to parse username");
        spnego_debug1("username is %s.", (const char *) user.data);
        spnego_log_krb5_error(kcontext, code);
        spnego_error(NGX_ERROR);
    }

    memset(&creds, 0, sizeof(creds));

    code = krb5_unparse_name(kcontext, client, &name);
    if (code) {
        spnego_log_error("Kerberos error: Cannot unparse username");
        spnego_log_krb5_error(kcontext, code);
        spnego_error(NGX_ERROR);
    }

    krb5_get_init_creds_opt_alloc(kcontext, &gic_options);

    code = krb5_get_init_creds_password(kcontext, &creds, client,
                (char *) r->headers_in.passwd.data,
                NULL, NULL, 0, NULL, gic_options);

    krb5_free_cred_contents(kcontext, &creds);

    if (code) {
        spnego_log_error("Kerberos error: Credentials failed");
        spnego_log_krb5_error(kcontext, code);
        spnego_error(NGX_DECLINED);
    }

    /* Try to add the system realm to $remote_user if needed. */
    if (alcf->fqun && !ngx_strchr(r->headers_in.user.data, '@')) {
#ifdef krb5_princ_realm
        /*
         * MIT does not have krb5_principal_get_realm() but its
         * krb5_princ_realm() is a macro that effectively points
         * to a char *.
         */
        const char *realm = krb5_princ_realm(kcontext, client)->data;
#else
        const char *realm = krb5_principal_get_realm(kcontext, client);
#endif
        if (realm) {
            new_user.len = r->headers_in.user.len + 1 + ngx_strlen(realm);
            new_user.data = ngx_palloc(r->pool, new_user.len);
            if (NULL == new_user.data) {
                spnego_log_error("Not enough memory");
                spnego_error(NGX_ERROR);
            }
            ngx_snprintf(new_user.data, new_user.len, "%V@%s",
                    &r->headers_in.user, realm);
            ngx_pfree(r->pool, r->headers_in.user.data);
            r->headers_in.user.data = new_user.data;
            r->headers_in.user.len = new_user.len;
        }
    }

    spnego_debug1("Setting $remote_user to %V", &r->headers_in.user);
    if (ngx_http_auth_spnego_set_bogus_authorization(r) != NGX_OK)
        spnego_log_error("Failed to set $remote_user");

    spnego_debug0("ngx_http_auth_spnego_basic: returning NGX_OK");

    ret = NGX_OK;

end:
    if (name)
        free(name);
    if (client)
        krb5_free_principal(kcontext, client);
    if (server)
        krb5_free_principal(kcontext, server);
    if (service.data)
        ngx_pfree(r->pool, service.data);
    if (user.data)
        ngx_pfree(r->pool, user.data);

    krb5_get_init_creds_opt_free(kcontext, gic_options);

    krb5_free_context(kcontext);

    return ret;
}