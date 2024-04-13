static uint32_t sn_coap_parser_options_parse_uint(uint8_t **packet_data_pptr, uint8_t option_len)
{
    uint32_t value = 0;
    while (option_len--) {
        value <<= 8;
        value |= *(*packet_data_pptr)++;
    }
    return value;
}