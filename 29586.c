SPICE_GNUC_VISIBLE int spice_server_migrate_info(SpiceServer *s, const char* dest,
                                          int port, int secure_port,
                                          const char* cert_subject)
{
    spice_info(NULL);
    spice_assert(!migration_interface);
    spice_assert(reds == s);

    if (!reds_set_migration_dest_info(dest, port, secure_port, cert_subject)) {
        return -1;
    }
    return 0;
}
