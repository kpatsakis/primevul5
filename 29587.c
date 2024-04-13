SPICE_GNUC_VISIBLE int spice_server_migrate_switch(SpiceServer *s)
{
    spice_assert(reds == s);
    spice_info(NULL);
    if (!reds->num_clients) {
       return 0;
    }
    reds->expect_migrate = FALSE;
    reds_mig_switch();
    return 0;
}
