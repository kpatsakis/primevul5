static unsigned calc_mac_hash(const uint8_t *mac)
{
    int index_bit, mac_bit;
    unsigned hash_index;

    hash_index = 0;
    mac_bit = 5;
    for (index_bit = 5; index_bit >= 0; index_bit--) {
        hash_index |= (get_bit(mac,  mac_bit) ^
                               get_bit(mac, mac_bit + 6) ^
                               get_bit(mac, mac_bit + 12) ^
                               get_bit(mac, mac_bit + 18) ^
                               get_bit(mac, mac_bit + 24) ^
                               get_bit(mac, mac_bit + 30) ^
                               get_bit(mac, mac_bit + 36) ^
                               get_bit(mac, mac_bit + 42)) << index_bit;
        mac_bit--;
    }

    return hash_index;
}