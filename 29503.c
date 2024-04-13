static void reds_handle_auth_sasl_step(void *opaque)
{
    const char *serverout;
    unsigned int serveroutlen;
    int err;
    char *clientdata = NULL;
    RedLinkInfo *link = (RedLinkInfo *)opaque;
    RedsSASL *sasl = &link->stream->sasl;
    uint32_t datalen = sasl->len;
    AsyncRead *obj = &link->async_read;

    /* NB, distinction of NULL vs "" is *critical* in SASL */
    if (datalen) {
        clientdata = sasl->data;
        clientdata[datalen - 1] = '\0'; /* Wire includes '\0', but make sure */
        datalen--; /* Don't count NULL byte when passing to _start() */
    }

    spice_info("Step using SASL Data %p (%d bytes)",
               clientdata, datalen);
    err = sasl_server_step(sasl->conn,
                           clientdata,
                           datalen,
                           &serverout,
                           &serveroutlen);
    if (err != SASL_OK &&
        err != SASL_CONTINUE) {
        spice_warning("sasl step failed %d (%s)",
                      err, sasl_errdetail(sasl->conn));
        goto authabort;
    }

    if (serveroutlen > SASL_DATA_MAX_LEN) {
        spice_warning("sasl step reply data too long %d",
                      serveroutlen);
        goto authabort;
    }

    spice_info("SASL return data %d bytes, %p", serveroutlen, serverout);

    if (serveroutlen) {
        serveroutlen += 1;
        sync_write(link->stream, &serveroutlen, sizeof(uint32_t));
        sync_write(link->stream, serverout, serveroutlen);
    } else {
        sync_write(link->stream, &serveroutlen, sizeof(uint32_t));
    }

    /* Whether auth is complete */
    sync_write_u8(link->stream, err == SASL_CONTINUE ? 0 : 1);

    if (err == SASL_CONTINUE) {
        spice_info("%s", "Authentication must continue (step)");
        /* Wait for step length */
        obj->now = (uint8_t *)&sasl->len;
        obj->end = obj->now + sizeof(uint32_t);
        obj->done = reds_handle_auth_sasl_steplen;
        async_read_handler(0, 0, &link->async_read);
    } else {
        int ssf;

        if (auth_sasl_check_ssf(sasl, &ssf) == 0) {
            spice_warning("Authentication rejected for weak SSF");
            goto authreject;
        }

        spice_info("Authentication successful");
        sync_write_u32(link->stream, SPICE_LINK_ERR_OK); /* Accept auth */

        /*
         * Delay writing in SSF encoded until now
         */
        sasl->runSSF = ssf;
        link->stream->writev = NULL; /* make sure writev isn't called directly anymore */

        reds_handle_link(link);
    }

    return;

authreject:
    sync_write_u32(link->stream, 1); /* Reject auth */
    sync_write_u32(link->stream, sizeof("Authentication failed"));
    sync_write(link->stream, "Authentication failed", sizeof("Authentication failed"));

authabort:
    reds_link_free(link);
    return;
}
