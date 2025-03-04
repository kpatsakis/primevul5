static int __init acpi_parse_hpet(unsigned long phys, unsigned long size)
{
	struct acpi_table_hpet *hpet_tbl;
	struct resource *hpet_res;
	resource_size_t res_start;

	if (!phys || !size)
		return -EINVAL;

	hpet_tbl = (struct acpi_table_hpet *)__acpi_map_table(phys, size);
	if (!hpet_tbl) {
		printk(KERN_WARNING PREFIX "Unable to map HPET\n");
		return -ENODEV;
	}

	if (hpet_tbl->addr.space_id != ACPI_SPACE_MEM) {
		printk(KERN_WARNING PREFIX "HPET timers must be located in "
		       "memory.\n");
		return -1;
	}

#define HPET_RESOURCE_NAME_SIZE 9
	hpet_res = alloc_bootmem(sizeof(*hpet_res) + HPET_RESOURCE_NAME_SIZE);
	if (hpet_res) {
		memset(hpet_res, 0, sizeof(*hpet_res));
		hpet_res->name = (void *)&hpet_res[1];
		hpet_res->flags = IORESOURCE_MEM | IORESOURCE_BUSY;
		snprintf((char *)hpet_res->name, HPET_RESOURCE_NAME_SIZE,
			 "HPET %u", hpet_tbl->number);
		hpet_res->end = (1 * 1024) - 1;
	}

#ifdef	CONFIG_X86_64
	vxtime.hpet_address = hpet_tbl->addr.addrl |
	    ((long)hpet_tbl->addr.addrh << 32);

	printk(KERN_INFO PREFIX "HPET id: %#x base: %#lx\n",
	       hpet_tbl->id, vxtime.hpet_address);

	res_start = vxtime.hpet_address;
#else				/* X86 */
	{
		extern unsigned long hpet_address;

		hpet_address = hpet_tbl->addr.addrl;
		printk(KERN_INFO PREFIX "HPET id: %#x base: %#lx\n",
		       hpet_tbl->id, hpet_address);

		res_start = hpet_address;
	}
#endif				/* X86 */

	if (hpet_res) {
		hpet_res->start = res_start;
		hpet_res->end += res_start;
		insert_resource(&iomem_resource, hpet_res);
	}

	return 0;
}