int acpi_map_lsapic(acpi_handle handle, int *pcpu)
{
	struct acpi_buffer buffer = { ACPI_ALLOCATE_BUFFER, NULL };
	union acpi_object *obj;
	struct acpi_table_lapic *lapic;
	cpumask_t tmp_map, new_map;
	u8 physid;
	int cpu;

	if (ACPI_FAILURE(acpi_evaluate_object(handle, "_MAT", NULL, &buffer)))
		return -EINVAL;

	if (!buffer.length || !buffer.pointer)
		return -EINVAL;

	obj = buffer.pointer;
	if (obj->type != ACPI_TYPE_BUFFER ||
	    obj->buffer.length < sizeof(*lapic)) {
		kfree(buffer.pointer);
		return -EINVAL;
	}

	lapic = (struct acpi_table_lapic *)obj->buffer.pointer;

	if ((lapic->header.type != ACPI_MADT_LAPIC) ||
	    (!lapic->flags.enabled)) {
		kfree(buffer.pointer);
		return -EINVAL;
	}

	physid = lapic->id;

	kfree(buffer.pointer);
	buffer.length = ACPI_ALLOCATE_BUFFER;
	buffer.pointer = NULL;

	tmp_map = cpu_present_map;
	mp_register_lapic(physid, lapic->flags.enabled);

	/*
	 * If mp_register_lapic successfully generates a new logical cpu
	 * number, then the following will get us exactly what was mapped
	 */
	cpus_andnot(new_map, cpu_present_map, tmp_map);
	if (cpus_empty(new_map)) {
		printk ("Unable to map lapic to logical cpu number\n");
		return -EINVAL;
	}

	cpu = first_cpu(new_map);

	*pcpu = cpu;
	return 0;
}