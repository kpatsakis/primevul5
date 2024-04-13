PJ_INLINE(pj_uint32_t) GETVAL32H(const pj_uint8_t *buf, unsigned pos)
{
    return (pj_uint32_t) ((buf[pos + 0] << 24UL) | \
	                  (buf[pos + 1] << 16UL) | \
	                  (buf[pos + 2] <<  8UL) | \
			  (buf[pos + 3] <<  0UL));
}