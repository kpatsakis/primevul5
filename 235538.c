static int8_t sn_coap_parser_check_packet_ptr(uint8_t *packet_data_ptr, uint8_t *packet_data_start_ptr, uint16_t packet_len, uint16_t delta)
{
    uint8_t *packet_end = packet_data_start_ptr + packet_len;
    uint8_t *new_data_ptr = packet_data_ptr + delta;

    if (delta > packet_len) {
        return -1;
    }

    if (new_data_ptr < packet_data_start_ptr ||
        new_data_ptr > packet_end) {
        return -1;
    }

    return 0;
}