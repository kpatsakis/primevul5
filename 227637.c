static uint64_t emac_read(void *opaque, hwaddr addr, unsigned int size)
{
    MSF2EmacState *s = opaque;
    uint32_t r = 0;

    addr >>= 2;

    switch (addr) {
    case R_DMA_IRQ:
        r = emac_get_isr(s);
        break;
    default:
        if (addr >= ARRAY_SIZE(s->regs)) {
            qemu_log_mask(LOG_GUEST_ERROR,
                          "%s: Bad offset 0x%" HWADDR_PRIx "\n", __func__,
                          addr * 4);
            return r;
        }
        r = s->regs[addr];
        break;
    }
    return r;
}