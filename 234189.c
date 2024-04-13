int acpi_register_gsi(u32 gsi, int triggering, int polarity)
{
	unsigned int irq;
	unsigned int plat_gsi = gsi;

#ifdef CONFIG_PCI
	/*
	 * Make sure all (legacy) PCI IRQs are set as level-triggered.
	 */
	if (acpi_irq_model == ACPI_IRQ_MODEL_PIC) {
		extern void eisa_set_level_irq(unsigned int irq);

		if (triggering == ACPI_LEVEL_SENSITIVE)
			eisa_set_level_irq(gsi);
	}
#endif

#ifdef CONFIG_X86_IO_APIC
	if (acpi_irq_model == ACPI_IRQ_MODEL_IOAPIC) {
		plat_gsi = mp_register_gsi(gsi, triggering, polarity);
	}
#endif
	acpi_gsi_to_irq(plat_gsi, &irq);
	return irq;
}