SPICE_GNUC_VISIBLE int spice_server_set_zlib_glz_compression(SpiceServer *s, spice_wan_compression_t comp)
{
    spice_assert(reds == s);
    if (comp == SPICE_WAN_COMPRESSION_INVALID) {
        spice_error("invalid zlib_glz state");
        return -1;
    }
    zlib_glz_state = comp;
    return 0;
}
