static int __init acpi_parse_madt(unsigned long phys_addr, unsigned long size)
{
	struct acpi_table_madt *madt = NULL;

	if (!phys_addr || !size || !cpu_has_apic)
		return -EINVAL;

	madt = (struct acpi_table_madt *)__acpi_map_table(phys_addr, size);
	if (!madt) {
		printk(KERN_WARNING PREFIX "Unable to map MADT\n");
		return -ENODEV;
	}

	if (madt->lapic_address) {
		acpi_lapic_addr = (u64) madt->lapic_address;

		printk(KERN_DEBUG PREFIX "Local APIC address 0x%08x\n",
		       madt->lapic_address);
	}

	acpi_madt_oem_check(madt->header.oem_id, madt->header.oem_table_id);

	return 0;
}