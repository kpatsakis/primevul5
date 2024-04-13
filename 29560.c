static void reds_start_auth_sasl(RedLinkInfo *link)
{
    const char *mechlist = NULL;
    sasl_security_properties_t secprops;
    int err;
    char *localAddr, *remoteAddr;
    int mechlistlen;
    AsyncRead *obj = &link->async_read;
    RedsSASL *sasl = &link->stream->sasl;

    /* Get local & remote client addresses in form  IPADDR;PORT */
    if (!(localAddr = addr_to_string("%s;%s", &link->stream->info->laddr_ext,
                                              link->stream->info->llen_ext))) {
        goto error;
    }

    if (!(remoteAddr = addr_to_string("%s;%s", &link->stream->info->paddr_ext,
                                               link->stream->info->plen_ext))) {
        free(localAddr);
        goto error;
    }

    err = sasl_server_new("spice",
                          NULL, /* FQDN - just delegates to gethostname */
                          NULL, /* User realm */
                          localAddr,
                          remoteAddr,
                          NULL, /* Callbacks, not needed */
                          SASL_SUCCESS_DATA,
                          &sasl->conn);
    free(localAddr);
    free(remoteAddr);
    localAddr = remoteAddr = NULL;

    if (err != SASL_OK) {
        spice_warning("sasl context setup failed %d (%s)",
                    err, sasl_errstring(err, NULL, NULL));
        sasl->conn = NULL;
        goto error;
    }

    /* Inform SASL that we've got an external SSF layer from TLS */
    if (link->stream->ssl) {
        sasl_ssf_t ssf;

        ssf = SSL_get_cipher_bits(link->stream->ssl, NULL);
        err = sasl_setprop(sasl->conn, SASL_SSF_EXTERNAL, &ssf);
        if (err != SASL_OK) {
            spice_warning("cannot set SASL external SSF %d (%s)",
                        err, sasl_errstring(err, NULL, NULL));
            goto error_dispose;
        }
    } else {
        sasl->wantSSF = 1;
    }

    memset(&secprops, 0, sizeof secprops);
    /* Inform SASL that we've got an external SSF layer from TLS */
    if (link->stream->ssl) {
        /* If we've got TLS (or UNIX domain sock), we don't care about SSF */
        secprops.min_ssf = 0;
        secprops.max_ssf = 0;
        secprops.maxbufsize = 8192;
        secprops.security_flags = 0;
    } else {
        /* Plain TCP, better get an SSF layer */
        secprops.min_ssf = 56; /* Good enough to require kerberos */
        secprops.max_ssf = 100000; /* Arbitrary big number */
        secprops.maxbufsize = 8192;
        /* Forbid any anonymous or trivially crackable auth */
        secprops.security_flags =
            SASL_SEC_NOANONYMOUS | SASL_SEC_NOPLAINTEXT;
    }

    err = sasl_setprop(sasl->conn, SASL_SEC_PROPS, &secprops);
    if (err != SASL_OK) {
        spice_warning("cannot set SASL security props %d (%s)",
                      err, sasl_errstring(err, NULL, NULL));
        goto error_dispose;
    }

    err = sasl_listmech(sasl->conn,
                        NULL, /* Don't need to set user */
                        "", /* Prefix */
                        ",", /* Separator */
                        "", /* Suffix */
                        &mechlist,
                        NULL,
                        NULL);
    if (err != SASL_OK || mechlist == NULL) {
        spice_warning("cannot list SASL mechanisms %d (%s)",
                      err, sasl_errdetail(sasl->conn));
        goto error_dispose;
    }

    spice_info("Available mechanisms for client: '%s'", mechlist);

    sasl->mechlist = spice_strdup(mechlist);

    mechlistlen = strlen(mechlist);
    if (!sync_write(link->stream, &mechlistlen, sizeof(uint32_t))
        || !sync_write(link->stream, sasl->mechlist, mechlistlen)) {
        spice_warning("SASL mechanisms write error");
        goto error;
    }

    spice_info("Wait for client mechname length");
    obj->now = (uint8_t *)&sasl->len;
    obj->end = obj->now + sizeof(uint32_t);
    obj->done = reds_handle_auth_mechlen;
    async_read_handler(0, 0, &link->async_read);

    return;

error_dispose:
    sasl_dispose(&sasl->conn);
    sasl->conn = NULL;
error:
    reds_link_free(link);
    return;
}
