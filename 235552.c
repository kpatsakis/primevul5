static uint16_t sn_coap_parser_move_packet_ptr(uint8_t **packet_data_pptr, uint8_t *packet_data_start_ptr, uint16_t packet_len, uint16_t delta)
{
    uint8_t *packet_end = packet_data_start_ptr + packet_len;
    uint8_t *new_data_ptr = *packet_data_pptr + delta;

    if (new_data_ptr < packet_data_start_ptr){
        return 0;
    } else if (new_data_ptr >= packet_end) {
        *packet_data_pptr = packet_end;
        return 0;
    }

    *packet_data_pptr = new_data_ptr;

    return (uint16_t)(packet_end - new_data_ptr);
}