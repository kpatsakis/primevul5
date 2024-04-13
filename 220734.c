void vnc_display_open(const char *id, Error **errp)
{
    VncDisplay *vs = vnc_display_find(id);
    QemuOpts *opts = qemu_opts_find(&qemu_vnc_opts, id);
    SocketAddress *saddr = NULL, *wsaddr = NULL;
    const char *share, *device_id;
    QemuConsole *con;
    bool password = false;
    bool reverse = false;
    const char *vnc;
    char *h;
    const char *credid;
    bool sasl = false;
#ifdef CONFIG_VNC_SASL
    int saslErr;
#endif
    int acl = 0;
    int lock_key_sync = 1;

    if (!vs) {
        error_setg(errp, "VNC display not active");
        return;
    }
    vnc_display_close(vs);

    if (!opts) {
        return;
    }
    vnc = qemu_opt_get(opts, "vnc");
    if (!vnc || strcmp(vnc, "none") == 0) {
        return;
    }

    h = strrchr(vnc, ':');
    if (h) {
        size_t hlen = h - vnc;

        const char *websocket = qemu_opt_get(opts, "websocket");
        int to = qemu_opt_get_number(opts, "to", 0);
        bool has_ipv4 = qemu_opt_get_bool(opts, "ipv4", false);
        bool has_ipv6 = qemu_opt_get_bool(opts, "ipv6", false);

        saddr = g_new0(SocketAddress, 1);
        if (websocket) {
            if (!qcrypto_hash_supports(QCRYPTO_HASH_ALG_SHA1)) {
                error_setg(errp,
                           "SHA1 hash support is required for websockets");
                goto fail;
            }

            wsaddr = g_new0(SocketAddress, 1);
            vs->ws_enabled = true;
        }

        if (strncmp(vnc, "unix:", 5) == 0) {
            saddr->type = SOCKET_ADDRESS_KIND_UNIX;
            saddr->u.q_unix = g_new0(UnixSocketAddress, 1);
            saddr->u.q_unix->path = g_strdup(vnc + 5);

            if (vs->ws_enabled) {
                error_setg(errp, "UNIX sockets not supported with websock");
                goto fail;
            }
        } else {
            unsigned long long baseport;
            saddr->type = SOCKET_ADDRESS_KIND_INET;
            saddr->u.inet = g_new0(InetSocketAddress, 1);
            if (vnc[0] == '[' && vnc[hlen - 1] == ']') {
                saddr->u.inet->host = g_strndup(vnc + 1, hlen - 2);
            } else {
                saddr->u.inet->host = g_strndup(vnc, hlen);
            }
            if (parse_uint_full(h + 1, &baseport, 10) < 0) {
                error_setg(errp, "can't convert to a number: %s", h + 1);
                goto fail;
            }
            if (baseport > 65535 ||
                baseport + 5900 > 65535) {
                error_setg(errp, "port %s out of range", h + 1);
                goto fail;
            }
            saddr->u.inet->port = g_strdup_printf(
                "%d", (int)baseport + 5900);

            if (to) {
                saddr->u.inet->has_to = true;
                saddr->u.inet->to = to + 5900;
            }
            saddr->u.inet->ipv4 = saddr->u.inet->has_ipv4 = has_ipv4;
            saddr->u.inet->ipv6 = saddr->u.inet->has_ipv6 = has_ipv6;

            if (vs->ws_enabled) {
                wsaddr->type = SOCKET_ADDRESS_KIND_INET;
                wsaddr->u.inet = g_new0(InetSocketAddress, 1);
                wsaddr->u.inet->host = g_strdup(saddr->u.inet->host);
                wsaddr->u.inet->port = g_strdup(websocket);

                if (to) {
                    wsaddr->u.inet->has_to = true;
                    wsaddr->u.inet->to = to;
                }
                wsaddr->u.inet->ipv4 = wsaddr->u.inet->has_ipv4 = has_ipv4;
                wsaddr->u.inet->ipv6 = wsaddr->u.inet->has_ipv6 = has_ipv6;
            }
        }
    } else {
        error_setg(errp, "no vnc port specified");
        goto fail;
    }

    password = qemu_opt_get_bool(opts, "password", false);
    if (password) {
        if (fips_get_state()) {
            error_setg(errp,
                       "VNC password auth disabled due to FIPS mode, "
                       "consider using the VeNCrypt or SASL authentication "
                       "methods as an alternative");
            goto fail;
        }
        if (!qcrypto_cipher_supports(
                QCRYPTO_CIPHER_ALG_DES_RFB)) {
            error_setg(errp,
                       "Cipher backend does not support DES RFB algorithm");
            goto fail;
        }
    }

    reverse = qemu_opt_get_bool(opts, "reverse", false);
    lock_key_sync = qemu_opt_get_bool(opts, "lock-key-sync", true);
    sasl = qemu_opt_get_bool(opts, "sasl", false);
#ifndef CONFIG_VNC_SASL
    if (sasl) {
        error_setg(errp, "VNC SASL auth requires cyrus-sasl support");
        goto fail;
    }
#endif /* CONFIG_VNC_SASL */
    credid = qemu_opt_get(opts, "tls-creds");
    if (credid) {
        Object *creds;
        if (qemu_opt_get(opts, "tls") ||
            qemu_opt_get(opts, "x509") ||
            qemu_opt_get(opts, "x509verify")) {
            error_setg(errp,
                       "'credid' parameter is mutually exclusive with "
                       "'tls', 'x509' and 'x509verify' parameters");
            goto fail;
        }

        creds = object_resolve_path_component(
            object_get_objects_root(), credid);
        if (!creds) {
            error_setg(errp, "No TLS credentials with id '%s'",
                       credid);
            goto fail;
        }
        vs->tlscreds = (QCryptoTLSCreds *)
            object_dynamic_cast(creds,
                                TYPE_QCRYPTO_TLS_CREDS);
        if (!vs->tlscreds) {
            error_setg(errp, "Object with id '%s' is not TLS credentials",
                       credid);
            goto fail;
        }
        object_ref(OBJECT(vs->tlscreds));

        if (vs->tlscreds->endpoint != QCRYPTO_TLS_CREDS_ENDPOINT_SERVER) {
            error_setg(errp,
                       "Expecting TLS credentials with a server endpoint");
            goto fail;
        }
    } else {
        const char *path;
        bool tls = false, x509 = false, x509verify = false;
        tls  = qemu_opt_get_bool(opts, "tls", false);
        if (tls) {
            path = qemu_opt_get(opts, "x509");

            if (path) {
                x509 = true;
            } else {
                path = qemu_opt_get(opts, "x509verify");
                if (path) {
                    x509 = true;
                    x509verify = true;
                }
            }
            vs->tlscreds = vnc_display_create_creds(x509,
                                                    x509verify,
                                                    path,
                                                    vs->id,
                                                    errp);
            if (!vs->tlscreds) {
                goto fail;
            }
        }
    }
    acl = qemu_opt_get_bool(opts, "acl", false);

    share = qemu_opt_get(opts, "share");
    if (share) {
        if (strcmp(share, "ignore") == 0) {
            vs->share_policy = VNC_SHARE_POLICY_IGNORE;
        } else if (strcmp(share, "allow-exclusive") == 0) {
            vs->share_policy = VNC_SHARE_POLICY_ALLOW_EXCLUSIVE;
        } else if (strcmp(share, "force-shared") == 0) {
            vs->share_policy = VNC_SHARE_POLICY_FORCE_SHARED;
        } else {
            error_setg(errp, "unknown vnc share= option");
            goto fail;
        }
    } else {
        vs->share_policy = VNC_SHARE_POLICY_ALLOW_EXCLUSIVE;
    }
    vs->connections_limit = qemu_opt_get_number(opts, "connections", 32);

#ifdef CONFIG_VNC_JPEG
    vs->lossy = qemu_opt_get_bool(opts, "lossy", false);
#endif
    vs->non_adaptive = qemu_opt_get_bool(opts, "non-adaptive", false);
    /* adaptive updates are only used with tight encoding and
     * if lossy updates are enabled so we can disable all the
     * calculations otherwise */
    if (!vs->lossy) {
        vs->non_adaptive = true;
    }

    if (acl) {
        if (strcmp(vs->id, "default") == 0) {
            vs->tlsaclname = g_strdup("vnc.x509dname");
        } else {
            vs->tlsaclname = g_strdup_printf("vnc.%s.x509dname", vs->id);
        }
        qemu_acl_init(vs->tlsaclname);
     }
#ifdef CONFIG_VNC_SASL
    if (acl && sasl) {
        char *aclname;

        if (strcmp(vs->id, "default") == 0) {
            aclname = g_strdup("vnc.username");
        } else {
            aclname = g_strdup_printf("vnc.%s.username", vs->id);
        }
        vs->sasl.acl = qemu_acl_init(aclname);
        g_free(aclname);
    }
#endif

    if (vnc_display_setup_auth(vs, password, sasl, vs->ws_enabled, errp) < 0) {
        goto fail;
    }

#ifdef CONFIG_VNC_SASL
    if ((saslErr = sasl_server_init(NULL, "qemu")) != SASL_OK) {
        error_setg(errp, "Failed to initialize SASL auth: %s",
                   sasl_errstring(saslErr, NULL, NULL));
        goto fail;
    }
#endif
    vs->lock_key_sync = lock_key_sync;

    device_id = qemu_opt_get(opts, "display");
    if (device_id) {
        DeviceState *dev;
        int head = qemu_opt_get_number(opts, "head", 0);

        dev = qdev_find_recursive(sysbus_get_default(), device_id);
        if (dev == NULL) {
            error_setg(errp, "Device '%s' not found", device_id);
            goto fail;
        }

        con = qemu_console_lookup_by_device(dev, head);
        if (con == NULL) {
            error_setg(errp, "Device %s is not bound to a QemuConsole",
                       device_id);
            goto fail;
        }
    } else {
        con = NULL;
    }

    if (con != vs->dcl.con) {
        unregister_displaychangelistener(&vs->dcl);
        vs->dcl.con = con;
        register_displaychangelistener(&vs->dcl);
    }

    if (reverse) {
        /* connect to viewer */
        int csock;
        vs->lsock = -1;
        vs->lwebsock = -1;
        if (vs->ws_enabled) {
            error_setg(errp, "Cannot use websockets in reverse mode");
            goto fail;
        }
        csock = socket_connect(saddr, errp, NULL, NULL);
        if (csock < 0) {
            goto fail;
        }
        vs->is_unix = saddr->type == SOCKET_ADDRESS_KIND_UNIX;
        vnc_connect(vs, csock, false, false);
    } else {
        /* listen for connects */
        vs->lsock = socket_listen(saddr, errp);
        if (vs->lsock < 0) {
            goto fail;
        }
        vs->is_unix = saddr->type == SOCKET_ADDRESS_KIND_UNIX;
        if (vs->ws_enabled) {
            vs->lwebsock = socket_listen(wsaddr, errp);
            if (vs->lwebsock < 0) {
                if (vs->lsock != -1) {
                    close(vs->lsock);
                    vs->lsock = -1;
                }
                goto fail;
            }
        }
        vs->enabled = true;
        qemu_set_fd_handler(vs->lsock, vnc_listen_regular_read, NULL, vs);
        if (vs->ws_enabled) {
            qemu_set_fd_handler(vs->lwebsock, vnc_listen_websocket_read,
                                NULL, vs);
        }
    }

    qapi_free_SocketAddress(saddr);
    qapi_free_SocketAddress(wsaddr);
    return;

fail:
    qapi_free_SocketAddress(saddr);
    qapi_free_SocketAddress(wsaddr);
    vs->enabled = false;
    vs->ws_enabled = false;
}