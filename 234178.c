acpi_parse_int_src_ovr(acpi_table_entry_header * header,
		       const unsigned long end)
{
	struct acpi_table_int_src_ovr *intsrc = NULL;

	intsrc = (struct acpi_table_int_src_ovr *)header;

	if (BAD_MADT_ENTRY(intsrc, end))
		return -EINVAL;

	acpi_table_print_madt_entry(header);

	if (intsrc->bus_irq == acpi_fadt.sci_int) {
		acpi_sci_ioapic_setup(intsrc->global_irq,
				      intsrc->flags.polarity,
				      intsrc->flags.trigger);
		return 0;
	}

	if (acpi_skip_timer_override &&
	    intsrc->bus_irq == 0 && intsrc->global_irq == 2) {
		printk(PREFIX "BIOS IRQ0 pin2 override ignored.\n");
		return 0;
	}

	mp_override_legacy_irq(intsrc->bus_irq,
			       intsrc->flags.polarity,
			       intsrc->flags.trigger, intsrc->global_irq);

	return 0;
}