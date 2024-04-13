static void sm501_palette_write(void *opaque, hwaddr addr,
                                uint32_t value)
{
    SM501State *s = (SM501State *)opaque;
    SM501_DPRINTF("sm501 palette write addr=%x, val=%x\n",
                  (int)addr, value);

    /* TODO : consider BYTE/WORD access */
    /* TODO : consider endian */

    assert(range_covers_byte(0, 0x400 * 3, addr));
    *(uint32_t *)&s->dc_palette[addr] = value;
    s->do_full_update = true;
}