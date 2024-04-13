static void GETVAL64H(const pj_uint8_t *buf, unsigned pos, pj_timestamp *ts)
{
    ts->u32.hi = GETVAL32H(buf, pos);
    ts->u32.lo = GETVAL32H(buf, pos+4);
}