static void set_halt(USBHostDevice *s, int ep)
{
    s->endp_table[ep - 1].halted = 1;
}