static void emac_store_desc(MSF2EmacState *s, EmacDesc *d, hwaddr desc)
{
    /* Convert from host endianness into LE. */
    d->pktaddr = cpu_to_le32(d->pktaddr);
    d->pktsize = cpu_to_le32(d->pktsize);
    d->next = cpu_to_le32(d->next);

    address_space_write(&s->dma_as, desc, MEMTXATTRS_UNSPECIFIED, d, sizeof *d);
}