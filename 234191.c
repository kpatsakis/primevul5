static int __init force_acpi_ht(struct dmi_system_id *d)
{
	if (!acpi_force) {
		printk(KERN_NOTICE "%s detected: force use of acpi=ht\n",
		       d->ident);
		disable_acpi();
		acpi_ht = 1;
	} else {
		printk(KERN_NOTICE
		       "Warning: acpi=force overrules DMI blacklist: acpi=ht\n");
	}
	return 0;
}