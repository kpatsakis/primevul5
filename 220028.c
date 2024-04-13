static uint64_t htonll(uint64_t n)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    uint32_t *i = (uint32_t*)&n;
    uint32_t b = i[0];
    i[0] = htonl(i[1]);
    i[1] = htonl(b);
#endif
    return n;
}