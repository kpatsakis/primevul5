SPICE_GNUC_VISIBLE int spice_server_set_image_compression(SpiceServer *s,
                                                          spice_image_compression_t comp)
{
    spice_assert(reds == s);
    set_image_compression(comp);
    return 0;
}
