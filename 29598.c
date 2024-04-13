SPICE_GNUC_VISIBLE void spice_server_set_name(SpiceServer *s, const char *name)
{
    free(spice_name);
    spice_name = spice_strdup(name);
}
