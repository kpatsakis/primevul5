SPICE_GNUC_VISIBLE int spice_server_is_server_mouse(SpiceServer *s)
{
    spice_assert(reds == s);
    return reds->mouse_mode == SPICE_MOUSE_MODE_SERVER;
}
