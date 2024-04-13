SPICE_GNUC_VISIBLE int spice_server_get_num_clients(SpiceServer *s)
{
    spice_assert(reds == s);
    return reds_num_of_clients();
}
