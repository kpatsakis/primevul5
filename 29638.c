static uint16_t full_header_get_msg_type(SpiceDataHeaderOpaque *header)
{
    return ((SpiceDataHeader *)header->data)->type;
}
