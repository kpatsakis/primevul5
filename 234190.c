static void __init acpi_process_madt(void)
{
#ifdef CONFIG_X86_LOCAL_APIC
	int count, error;

	count = acpi_table_parse(ACPI_APIC, acpi_parse_madt);
	if (count >= 1) {

		/*
		 * Parse MADT LAPIC entries
		 */
		error = acpi_parse_madt_lapic_entries();
		if (!error) {
			acpi_lapic = 1;

#ifdef CONFIG_X86_GENERICARCH
			generic_bigsmp_probe();
#endif
			/*
			 * Parse MADT IO-APIC entries
			 */
			error = acpi_parse_madt_ioapic_entries();
			if (!error) {
				acpi_irq_model = ACPI_IRQ_MODEL_IOAPIC;
				acpi_irq_balance_set(NULL);
				acpi_ioapic = 1;

				smp_found_config = 1;
				clustered_apic_check();
			}
		}
		if (error == -EINVAL) {
			/*
			 * Dell Precision Workstation 410, 610 come here.
			 */
			printk(KERN_ERR PREFIX
			       "Invalid BIOS MADT, disabling ACPI\n");
			disable_acpi();
		}
	}
#endif
	return;
}