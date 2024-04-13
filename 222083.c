static int is_halted(USBHostDevice *s, int ep)
{
    return s->endp_table[ep - 1].halted;
}