SPICE_GNUC_VISIBLE spice_image_compression_t spice_server_get_image_compression(SpiceServer *s)
{
    spice_assert(reds == s);
    return image_compression;
}
