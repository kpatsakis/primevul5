static uint32_t full_header_get_msg_size(SpiceDataHeaderOpaque *header)
{
    return ((SpiceDataHeader *)header->data)->size;
}
