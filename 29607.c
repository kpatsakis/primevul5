SPICE_GNUC_VISIBLE int spice_server_set_tls(SpiceServer *s, int port,
                                            const char *ca_cert_file, const char *certs_file,
                                            const char *private_key_file, const char *key_passwd,
                                            const char *dh_key_file, const char *ciphersuite)
{
    spice_assert(reds == s);
    if (port == 0 || ca_cert_file == NULL || certs_file == NULL ||
        private_key_file == NULL) {
        return -1;
    }
    if (port < 0 || port > 0xffff) {
        return -1;
    }
    memset(&ssl_parameters, 0, sizeof(ssl_parameters));

    spice_secure_port = port;
    g_strlcpy(ssl_parameters.ca_certificate_file, ca_cert_file,
              sizeof(ssl_parameters.ca_certificate_file));
    g_strlcpy(ssl_parameters.certs_file, certs_file,
              sizeof(ssl_parameters.certs_file));
    g_strlcpy(ssl_parameters.private_key_file, private_key_file,
              sizeof(ssl_parameters.private_key_file));

    if (key_passwd) {
        g_strlcpy(ssl_parameters.keyfile_password, key_passwd,
                  sizeof(ssl_parameters.keyfile_password));
    }
    if (ciphersuite) {
        g_strlcpy(ssl_parameters.ciphersuite, ciphersuite,
                  sizeof(ssl_parameters.ciphersuite));
    }
    if (dh_key_file) {
        g_strlcpy(ssl_parameters.dh_key_file, dh_key_file,
                  sizeof(ssl_parameters.dh_key_file));
    }
    return 0;
}
