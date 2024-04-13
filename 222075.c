static void clear_halt(USBHostDevice *s, int ep)
{
    s->endp_table[ep - 1].halted = 0;
}