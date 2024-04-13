ngx_http_auth_spnego_auth_user_gss(
        ngx_http_request_t * r,
        ngx_http_auth_spnego_ctx_t * ctx,
        ngx_http_auth_spnego_loc_conf_t * alcf)
{
    ngx_int_t ret = NGX_DECLINED;
    u_char *pu;
    ngx_str_t spnego_token = ngx_null_string;
    OM_uint32 major_status, minor_status, minor_status2;
    gss_buffer_desc service = GSS_C_EMPTY_BUFFER;
    gss_name_t my_gss_name = GSS_C_NO_NAME;
    gss_cred_id_t my_gss_creds = GSS_C_NO_CREDENTIAL;
    gss_buffer_desc input_token = GSS_C_EMPTY_BUFFER;
    gss_ctx_id_t gss_context = GSS_C_NO_CONTEXT;
    gss_name_t client_name = GSS_C_NO_NAME;
    gss_buffer_desc output_token = GSS_C_EMPTY_BUFFER;

    if (NULL == ctx || ctx->token.len == 0)
        return ret;

    spnego_debug0("GSSAPI authorizing");

    if (!use_keytab(r, &alcf->keytab)) {
        spnego_debug0("Failed to specify keytab");
        spnego_error(NGX_ERROR);
    }

    if (alcf->srvcname.len > 0) {
        /* if there is a specific service prinicipal set in the configuration
         * file, we need to use it.  Otherwise, use the default of no credentials
         */
        service.length = alcf->srvcname.len + alcf->realm.len + 2;
        service.value = ngx_palloc(r->pool, service.length);
        if (NULL == service.value) {
            spnego_error(NGX_ERROR);
        }
        ngx_snprintf(service.value, service.length, "%V@%V%Z",
                &alcf->srvcname, &alcf->realm);

        spnego_debug1("Using service principal: %s", service.value);
        major_status = gss_import_name(&minor_status, &service,
                (gss_OID) GSS_KRB5_NT_PRINCIPAL_NAME, &my_gss_name);
        if (GSS_ERROR(major_status)) {
            spnego_log_error("%s Used service principal: %s", get_gss_error(
                        r->pool, minor_status, "gss_import_name() failed"),
                    (u_char *) service.value);
            spnego_error(NGX_ERROR);
        }
        gss_buffer_desc human_readable_gss_name = GSS_C_EMPTY_BUFFER;
        major_status = gss_display_name(&minor_status, my_gss_name,
                &human_readable_gss_name, NULL);

        if (GSS_ERROR(major_status)) {
            spnego_log_error("%s Used service principal: %s ", get_gss_error(
                        r->pool, minor_status, "gss_display_name() failed"),
                    (u_char *) service.value);
        }
        spnego_debug1("my_gss_name %s", human_readable_gss_name.value);

        /* Obtain credentials */
        major_status = gss_acquire_cred(&minor_status, my_gss_name,
                GSS_C_INDEFINITE, GSS_C_NO_OID_SET, GSS_C_ACCEPT, &my_gss_creds,
                NULL, NULL);
        if (GSS_ERROR(major_status)) {
            spnego_log_error("%s Used service principal: %s", get_gss_error(
                        r->pool, minor_status, "gss_acquire_cred() failed"),
                    (u_char *) service.value);
            spnego_error(NGX_ERROR);
        }

    }

    input_token.length = ctx->token.len;
    input_token.value = (void *) ctx->token.data;

    major_status = gss_accept_sec_context(&minor_status, &gss_context,
            my_gss_creds, &input_token, GSS_C_NO_CHANNEL_BINDINGS, &client_name,
            NULL, &output_token, NULL, NULL, NULL);
    if (GSS_ERROR(major_status)) {
        spnego_debug1("%s", get_gss_error(
                    r->pool, minor_status, "gss_accept_sec_context() failed"));
        spnego_error(NGX_DECLINED);
    }

    if (major_status & GSS_S_CONTINUE_NEEDED) {
        spnego_debug0("only one authentication iteration allowed");
        spnego_error(NGX_DECLINED);
    }

    if (output_token.length) {
        spnego_token.data = (u_char *) output_token.value;
        spnego_token.len = output_token.length;

        ctx->token_out_b64.len = ngx_base64_encoded_length(spnego_token.len);
        ctx->token_out_b64.data = ngx_pcalloc(r->pool, ctx->token_out_b64.len + 1);
        if (NULL == ctx->token_out_b64.data) {
            spnego_log_error("Not enough memory");
            gss_release_buffer(&minor_status2, &output_token);
            spnego_error(NGX_ERROR);
        }
        ngx_encode_base64(&ctx->token_out_b64, &spnego_token);
        gss_release_buffer(&minor_status2, &output_token);
    }
	else {
        ctx->token_out_b64.len = 0;
	}

    /* getting user name at the other end of the request */
    major_status = gss_display_name(&minor_status, client_name, &output_token, NULL);
    if (GSS_ERROR(major_status)) {
        spnego_log_error("%s", get_gss_error(r->pool, minor_status,
                    "gss_display_name() failed"));
        spnego_error(NGX_ERROR);
    }

    if (output_token.length) {
        /* Apply local rules to map Kerberos Principals to short names */
        if (alcf->map_to_local) {
            gss_OID mech_type = discard_const(gss_mech_krb5);
            output_token = (gss_buffer_desc) GSS_C_EMPTY_BUFFER;
            major_status = gss_localname(&minor_status, client_name,
                    mech_type, &output_token);
            if (GSS_ERROR(major_status)) {
                spnego_log_error("%s", get_gss_error(r->pool, minor_status,
                            "gss_localname() failed"));
                spnego_error(NGX_ERROR);
            }
        }

        /* TOFIX dirty quick trick for now (no "-1" i.e. include '\0' */
        ngx_str_t user = {
            output_token.length,
            (u_char *) output_token.value
        };

        r->headers_in.user.data = ngx_pstrdup(r->pool, &user);
        if (NULL == r->headers_in.user.data) {
            spnego_log_error("ngx_pstrdup failed to allocate");
            spnego_error(NGX_ERROR);
        }

        r->headers_in.user.len = user.len;
        if (alcf->fqun == 0) {
            pu = ngx_strlchr(r->headers_in.user.data,
                    r->headers_in.user.data + r->headers_in.user.len, '@');
            if (pu != NULL && ngx_strncmp(pu + 1, alcf->realm.data, alcf->realm.len) == 0) {
                *pu = '\0';
                r->headers_in.user.len = ngx_strlen(r->headers_in.user.data);
            }
        }

        /* this for the sake of ngx_http_variable_remote_user */
        if (ngx_http_auth_spnego_set_bogus_authorization(r) != NGX_OK) {
            spnego_log_error("Failed to set remote_user");
        }
        spnego_debug1("user is %V", &r->headers_in.user);
    }

    gss_release_buffer(&minor_status, &output_token);

    ret = NGX_OK;
    goto end;

end:
    if (output_token.length)
        gss_release_buffer(&minor_status, &output_token);

    if (client_name != GSS_C_NO_NAME)
        gss_release_name(&minor_status, &client_name);

    if (gss_context != GSS_C_NO_CONTEXT)
        gss_delete_sec_context(&minor_status, &gss_context,
                GSS_C_NO_BUFFER);

    if (my_gss_name != GSS_C_NO_NAME)
        gss_release_name(&minor_status, &my_gss_name);

    if (my_gss_creds != GSS_C_NO_CREDENTIAL)
        gss_release_cred(&minor_status, &my_gss_creds);

    return ret;
}