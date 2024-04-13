SPICE_GNUC_VISIBLE int spice_server_set_sasl_appname(SpiceServer *s, const char *appname)
{
    spice_assert(reds == s);
#if HAVE_SASL
    free(sasl_appname);
    sasl_appname = spice_strdup(appname);
    return 0;
#else
    return -1;
#endif
}
