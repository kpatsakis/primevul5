int acpi_gsi_to_irq(u32 gsi, unsigned int *irq)
{
#ifdef CONFIG_X86_IO_APIC
	if (use_pci_vector() && !platform_legacy_irq(gsi))
		*irq = IO_APIC_VECTOR(gsi);
	else
#endif
		*irq = gsi_irq_sharing(gsi);
	return 0;
}