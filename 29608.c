SPICE_GNUC_VISIBLE void spice_server_set_uuid(SpiceServer *s, const uint8_t uuid[16])
{
    memcpy(spice_uuid, uuid, sizeof(spice_uuid));
    spice_uuid_is_set = TRUE;
}
