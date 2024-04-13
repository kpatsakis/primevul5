static int8_t sn_coap_parser_read_packet_u8(uint8_t *dst, uint8_t *packet_data_ptr, uint8_t *packet_data_start_ptr, uint16_t packet_len)
{
    int8_t ptr_check_result;

    ptr_check_result = sn_coap_parser_check_packet_ptr(packet_data_ptr, packet_data_start_ptr, packet_len, 1);

    if (ptr_check_result != 0) {
        return ptr_check_result;
    }

    *dst = *packet_data_ptr;

    return 0;
}