SPICE_GNUC_VISIBLE int spice_server_set_playback_compression(SpiceServer *s, int enable)
{
    spice_assert(reds == s);
    snd_set_playback_compression(enable);
    return 0;
}
