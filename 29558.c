static int reds_set_migration_dest_info(const char* dest,
                                        int port, int secure_port,
                                        const char* cert_subject)
{
    RedsMigSpice *spice_migration = NULL;

    reds_mig_release();
    if ((port == -1 && secure_port == -1) || !dest) {
        return FALSE;
    }

    spice_migration = spice_new0(RedsMigSpice, 1);
    spice_migration->port = port;
    spice_migration->sport = secure_port;
    spice_migration->host = spice_strdup(dest);
    if (cert_subject) {
        spice_migration->cert_subject = spice_strdup(cert_subject);
    }

    reds->mig_spice = spice_migration;

    return TRUE;
}
