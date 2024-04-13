static void gem_get_rx_desc(CadenceGEMState *s, int q)
{
    hwaddr desc_addr = gem_get_rx_desc_addr(s, q);

    DB_PRINT("read descriptor 0x%" HWADDR_PRIx "\n", desc_addr);

    /* read current descriptor */
    address_space_read(&s->dma_as, desc_addr, MEMTXATTRS_UNSPECIFIED,
                       s->rx_desc[q],
                       sizeof(uint32_t) * gem_get_desc_len(s, true));

    /* Descriptor owned by software ? */
    if (rx_desc_get_ownership(s->rx_desc[q]) == 1) {
        DB_PRINT("descriptor 0x%" HWADDR_PRIx " owned by sw.\n", desc_addr);
        s->regs[GEM_RXSTATUS] |= GEM_RXSTATUS_NOBUF;
        gem_set_isr(s, q, GEM_INT_RXUSED);
        /* Handle interrupt consequences */
        gem_update_int_status(s);
    }
}