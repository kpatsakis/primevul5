SPICE_GNUC_VISIBLE int spice_server_set_port(SpiceServer *s, int port)
{
    spice_assert(reds == s);
    if (port < 0 || port > 0xffff) {
        return -1;
    }
    spice_port = port;
    return 0;
}
