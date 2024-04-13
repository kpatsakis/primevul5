acpi_parse_lapic(acpi_table_entry_header * header, const unsigned long end)
{
	struct acpi_table_lapic *processor = NULL;

	processor = (struct acpi_table_lapic *)header;

	if (BAD_MADT_ENTRY(processor, end))
		return -EINVAL;

	acpi_table_print_madt_entry(header);

	/* Record local apic id only when enabled */
	if (processor->flags.enabled)
		x86_acpiid_to_apicid[processor->acpi_id] = processor->id;

	/*
	 * We need to register disabled CPU as well to permit
	 * counting disabled CPUs. This allows us to size
	 * cpus_possible_map more accurately, to permit
	 * to not preallocating memory for all NR_CPUS
	 * when we use CPU hotplug.
	 */
	mp_register_lapic(processor->id,	/* APIC ID */
			  processor->flags.enabled);	/* Enabled? */

	return 0;
}