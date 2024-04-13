count_utf8_lead_bytes_with_word(const VALUE *s)
{
    VALUE d = *s;
    d |= ~(d>>1);
    d >>= 6;
    d &= NONASCII_MASK >> 7;
    d += (d>>8);
    d += (d>>16);
#if SIZEOF_VALUE == 8
    d += (d>>32);
#endif
    return (d&0xF);
}