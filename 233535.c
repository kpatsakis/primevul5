download_data( struct net_device  *dev,  u32  *crc_p )
{
	struct net_local  *nl    = (struct net_local *) dev->priv;
	struct sk_buff    *skb	 = nl->tx_buf_p;

	unsigned  len = min_t(unsigned int, skb->len - nl->outpos, nl->framelen);

	outsb( dev->base_addr + DAT, skb->data + nl->outpos, len );
	*crc_p = calc_crc32( *crc_p, skb->data + nl->outpos, len );

	/* if packet too short we should write some more bytes to pad */
	for( len = nl->framelen - len;  len--; )
		outb( 0, dev->base_addr + DAT ),
		*crc_p = CRC32( 0, *crc_p );
}