static uint16_t mini_header_get_msg_type(SpiceDataHeaderOpaque *header)
{
    return ((SpiceMiniDataHeader *)header->data)->type;
}
