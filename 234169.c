static int __init acpi_parse_sbf(unsigned long phys_addr, unsigned long size)
{
	struct acpi_table_sbf *sb;

	if (!phys_addr || !size)
		return -EINVAL;

	sb = (struct acpi_table_sbf *)__acpi_map_table(phys_addr, size);
	if (!sb) {
		printk(KERN_WARNING PREFIX "Unable to map SBF\n");
		return -ENODEV;
	}

	sbf_port = sb->sbf_cmos;	/* Save CMOS port */

	return 0;
}