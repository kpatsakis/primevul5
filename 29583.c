SPICE_GNUC_VISIBLE int spice_server_migrate_client_state(SpiceServer *s)
{
    spice_assert(reds == s);

    if (!reds_main_channel_connected()) {
        return SPICE_MIGRATE_CLIENT_NONE;
    } else if (reds->mig_wait_connect) {
        return SPICE_MIGRATE_CLIENT_WAITING;
    } else {
        return SPICE_MIGRATE_CLIENT_READY;
    }
    return 0;
}
