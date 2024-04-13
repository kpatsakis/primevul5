sbni_pci_probe( struct net_device  *dev )
{
	struct pci_dev  *pdev = NULL;

	while( (pdev = pci_get_class( PCI_CLASS_NETWORK_OTHER << 8, pdev ))
	       != NULL ) {
		int  pci_irq_line;
		unsigned long  pci_ioaddr;
		u16  subsys;

		if( pdev->vendor != SBNI_PCI_VENDOR
		    &&  pdev->device != SBNI_PCI_DEVICE )
				continue;

		pci_ioaddr = pci_resource_start( pdev, 0 );
		pci_irq_line = pdev->irq;

		/* Avoid already found cards from previous calls */
		if( !request_region( pci_ioaddr, SBNI_IO_EXTENT, dev->name ) ) {
			pci_read_config_word( pdev, PCI_SUBSYSTEM_ID, &subsys );

			if (subsys != 2)
				continue;

			/* Dual adapter is present */
			if (!request_region(pci_ioaddr += 4, SBNI_IO_EXTENT,
							dev->name ) )
				continue;
		}

		if( pci_irq_line <= 0  ||  pci_irq_line >= NR_IRQS )
			printk( KERN_WARNING "  WARNING: The PCI BIOS assigned "
				"this PCI card to IRQ %d, which is unlikely "
				"to work!.\n"
				KERN_WARNING " You should use the PCI BIOS "
				"setup to assign a valid IRQ line.\n",
				pci_irq_line );

		/* avoiding re-enable dual adapters */
		if( (pci_ioaddr & 7) == 0  &&  pci_enable_device( pdev ) ) {
			release_region( pci_ioaddr, SBNI_IO_EXTENT );
			pci_dev_put( pdev );
			return  -EIO;
		}
		if( sbni_probe1( dev, pci_ioaddr, pci_irq_line ) ) {
			SET_NETDEV_DEV(dev, &pdev->dev);
			/* not the best thing to do, but this is all messed up 
			   for hotplug systems anyway... */
			pci_dev_put( pdev );
			return  0;
		}
	}
	return  -ENODEV;
}