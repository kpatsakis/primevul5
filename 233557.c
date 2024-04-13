sbni_probe1( struct net_device  *dev,  unsigned long  ioaddr,  int  irq )
{
	struct net_local  *nl;

	if( sbni_card_probe( ioaddr ) ) {
		release_region( ioaddr, SBNI_IO_EXTENT );
		return NULL;
	}

	outb( 0, ioaddr + CSR0 );

	if( irq < 2 ) {
		unsigned long irq_mask;

		irq_mask = probe_irq_on();
		outb( EN_INT | TR_REQ, ioaddr + CSR0 );
		outb( PR_RES, ioaddr + CSR1 );
		mdelay(50);
		irq = probe_irq_off(irq_mask);
		outb( 0, ioaddr + CSR0 );

		if( !irq ) {
			printk( KERN_ERR "%s: can't detect device irq!\n",
				dev->name );
			release_region( ioaddr, SBNI_IO_EXTENT );
			return NULL;
		}
	} else if( irq == 2 )
		irq = 9;

	dev->irq = irq;
	dev->base_addr = ioaddr;

	/* Allocate dev->priv and fill in sbni-specific dev fields. */
	nl = dev->priv;
	if( !nl ) {
		printk( KERN_ERR "%s: unable to get memory!\n", dev->name );
		release_region( ioaddr, SBNI_IO_EXTENT );
		return NULL;
	}

	dev->priv = nl;
	memset( nl, 0, sizeof(struct net_local) );
	spin_lock_init( &nl->lock );

	/* store MAC address (generate if that isn't known) */
	*(__be16 *)dev->dev_addr = htons( 0x00ff );
	*(__be32 *)(dev->dev_addr + 2) = htonl( 0x01000000 |
		( (mac[num]  ?  mac[num]  :  (u32)((long)dev->priv)) & 0x00ffffff) );

	/* store link settings (speed, receive level ) */
	nl->maxframe  = DEFAULT_FRAME_LEN;
	nl->csr1.rate = baud[ num ];

	if( (nl->cur_rxl_index = rxl[ num ]) == -1 )
		/* autotune rxl */
		nl->cur_rxl_index = DEF_RXL,
		nl->delta_rxl = DEF_RXL_DELTA;
	else
		nl->delta_rxl = 0;
	nl->csr1.rxl  = rxl_tab[ nl->cur_rxl_index ];
	if( inb( ioaddr + CSR0 ) & 0x01 )
		nl->state |= FL_SLOW_MODE;

	printk( KERN_NOTICE "%s: ioaddr %#lx, irq %d, "
		"MAC: 00:ff:01:%02x:%02x:%02x\n", 
		dev->name, dev->base_addr, dev->irq,
		((u8 *) dev->dev_addr) [3],
		((u8 *) dev->dev_addr) [4],
		((u8 *) dev->dev_addr) [5] );

	printk( KERN_NOTICE "%s: speed %d, receive level ", dev->name,
		( (nl->state & FL_SLOW_MODE)  ?  500000 : 2000000)
		/ (1 << nl->csr1.rate) );

	if( nl->delta_rxl == 0 )
		printk( "0x%x (fixed)\n", nl->cur_rxl_index ); 
	else
		printk( "(auto)\n");

#ifdef CONFIG_SBNI_MULTILINE
	nl->master = dev;
	nl->link   = NULL;
#endif
   
	sbni_cards[ num++ ] = dev;
	return  dev;
}