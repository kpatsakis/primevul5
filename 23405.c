static unsigned get_bit(const uint8_t *mac, unsigned bit)
{
    unsigned byte;

    byte = mac[bit / 8];
    byte >>= (bit & 0x7);
    byte &= 1;

    return byte;
}