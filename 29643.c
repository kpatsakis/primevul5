static uint32_t mini_header_get_msg_size(SpiceDataHeaderOpaque *header)
{
    return ((SpiceMiniDataHeader *)header->data)->size;
}
