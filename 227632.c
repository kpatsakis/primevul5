static void emac_load_desc(MSF2EmacState *s, EmacDesc *d, hwaddr desc)
{
    address_space_read(&s->dma_as, desc, MEMTXATTRS_UNSPECIFIED, d, sizeof *d);
    /* Convert from LE into host endianness. */
    d->pktaddr = le32_to_cpu(d->pktaddr);
    d->pktsize = le32_to_cpu(d->pktsize);
    d->next = le32_to_cpu(d->next);
}