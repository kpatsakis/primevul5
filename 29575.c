SPICE_GNUC_VISIBLE void spice_server_destroy(SpiceServer *s)
{
    spice_assert(reds == s);
    reds_exit();
}
