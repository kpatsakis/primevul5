SPICE_GNUC_VISIBLE int spice_server_set_exit_on_disconnect(SpiceServer *s, int flag)
{
    spice_assert(reds == s);
    exit_on_disconnect = !!flag;
    return 0;
}
