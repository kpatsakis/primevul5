MemTxResult address_space_set(AddressSpace *as, hwaddr addr,
                              uint8_t c, hwaddr len, MemTxAttrs attrs)
{
#define FILLBUF_SIZE 512
    uint8_t fillbuf[FILLBUF_SIZE];
    int l;
    MemTxResult error = MEMTX_OK;

    memset(fillbuf, c, FILLBUF_SIZE);
    while (len > 0) {
        l = len < FILLBUF_SIZE ? len : FILLBUF_SIZE;
        error |= address_space_write(as, addr, attrs, fillbuf, l);
        len -= l;
        addr += l;
    }

    return error;
}