vnc_display_create_creds(bool x509,
                         bool x509verify,
                         const char *dir,
                         const char *id,
                         Error **errp)
{
    gchar *credsid = g_strdup_printf("tlsvnc%s", id);
    Object *parent = object_get_objects_root();
    Object *creds;
    Error *err = NULL;

    if (x509) {
        creds = object_new_with_props(TYPE_QCRYPTO_TLS_CREDS_X509,
                                      parent,
                                      credsid,
                                      &err,
                                      "endpoint", "server",
                                      "dir", dir,
                                      "verify-peer", x509verify ? "yes" : "no",
                                      NULL);
    } else {
        creds = object_new_with_props(TYPE_QCRYPTO_TLS_CREDS_ANON,
                                      parent,
                                      credsid,
                                      &err,
                                      "endpoint", "server",
                                      NULL);
    }

    g_free(credsid);

    if (err) {
        error_propagate(errp, err);
        return NULL;
    }

    return QCRYPTO_TLS_CREDS(creds);
}