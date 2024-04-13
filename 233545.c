sbni_isa_probe( struct net_device  *dev )
{
	if( dev->base_addr > 0x1ff
	    &&  request_region( dev->base_addr, SBNI_IO_EXTENT, dev->name )
	    &&  sbni_probe1( dev, dev->base_addr, dev->irq ) )

		return  0;
	else {
		printk( KERN_ERR "sbni: base address 0x%lx is busy, or adapter "
			"is malfunctional!\n", dev->base_addr );
		return  -ENODEV;
	}
}