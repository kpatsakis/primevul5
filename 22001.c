static void PUTVAL16H(pj_uint8_t *buf, unsigned pos, pj_uint16_t hval)
{
    buf[pos+0] = (pj_uint8_t) ((hval & 0xFF00) >> 8);
    buf[pos+1] = (pj_uint8_t) ((hval & 0x00FF) >> 0);
}