SPICE_GNUC_VISIBLE int spice_server_set_noauth(SpiceServer *s)
{
    spice_assert(reds == s);
    memset(taTicket.password, 0, sizeof(taTicket.password));
    ticketing_enabled = 0;
    return 0;
}
