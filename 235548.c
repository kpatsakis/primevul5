static int8_t sn_coap_parser_read_packet_u16(uint16_t *dst, uint8_t *packet_data_ptr, uint8_t *packet_data_start_ptr, uint16_t packet_len)
{
    int8_t ptr_check_result;
    uint16_t value;

    ptr_check_result = sn_coap_parser_check_packet_ptr(packet_data_ptr, packet_data_start_ptr, packet_len, 2);

    if (ptr_check_result != 0) {
        return ptr_check_result;
    }

    value = *(packet_data_ptr) << 8;
    value |= *(packet_data_ptr + 1);
    *dst = value;

    return 0;
}