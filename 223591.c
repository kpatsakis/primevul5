static uint32_t sm501_palette_read(void *opaque, hwaddr addr)
{
    SM501State *s = (SM501State *)opaque;
    SM501_DPRINTF("sm501 palette read addr=%x\n", (int)addr);

    /* TODO : consider BYTE/WORD access */
    /* TODO : consider endian */

    assert(range_covers_byte(0, 0x400 * 3, addr));
    return *(uint32_t *)&s->dc_palette[addr];
}