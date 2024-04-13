int __init acpi_parse_mcfg(unsigned long phys_addr, unsigned long size)
{
	struct acpi_table_mcfg *mcfg;
	unsigned long i;
	int config_size;

	if (!phys_addr || !size)
		return -EINVAL;

	mcfg = (struct acpi_table_mcfg *)__acpi_map_table(phys_addr, size);
	if (!mcfg) {
		printk(KERN_WARNING PREFIX "Unable to map MCFG\n");
		return -ENODEV;
	}

	/* how many config structures do we have */
	pci_mmcfg_config_num = 0;
	i = size - sizeof(struct acpi_table_mcfg);
	while (i >= sizeof(struct acpi_table_mcfg_config)) {
		++pci_mmcfg_config_num;
		i -= sizeof(struct acpi_table_mcfg_config);
	};
	if (pci_mmcfg_config_num == 0) {
		printk(KERN_ERR PREFIX "MMCONFIG has no entries\n");
		return -ENODEV;
	}

	config_size = pci_mmcfg_config_num * sizeof(*pci_mmcfg_config);
	pci_mmcfg_config = kmalloc(config_size, GFP_KERNEL);
	if (!pci_mmcfg_config) {
		printk(KERN_WARNING PREFIX
		       "No memory for MCFG config tables\n");
		return -ENOMEM;
	}

	memcpy(pci_mmcfg_config, &mcfg->config, config_size);
	for (i = 0; i < pci_mmcfg_config_num; ++i) {
		if (mcfg->config[i].base_reserved) {
			printk(KERN_ERR PREFIX
			       "MMCONFIG not in low 4GB of memory\n");
			kfree(pci_mmcfg_config);
			pci_mmcfg_config_num = 0;
			return -ENODEV;
		}
	}

	return 0;
}