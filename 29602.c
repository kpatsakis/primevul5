SPICE_GNUC_VISIBLE int spice_server_set_sasl(SpiceServer *s, int enabled)
{
    spice_assert(reds == s);
#if HAVE_SASL
    sasl_enabled = enabled;
    return 0;
#else
    return -1;
#endif
}
