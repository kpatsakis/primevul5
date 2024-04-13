static inline void gzip_header(void *buf)
{
    uint8_t *p;

    /* GZip Magic bytes */
    p = buf;
    *p++ = 0x1F;
    *p++ = 0x8B;
    *p++ = 8;
    *p++ = 0;
    *p++ = 0;
    *p++ = 0;
    *p++ = 0;
    *p++ = 0;
    *p++ = 0;
    *p++ = 0xFF;
}