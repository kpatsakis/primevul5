int acpi_unmap_lsapic(int cpu)
{
	int i;

	for_each_possible_cpu(i) {
		if (x86_acpiid_to_apicid[i] == x86_cpu_to_apicid[cpu]) {
			x86_acpiid_to_apicid[i] = -1;
			break;
		}
	}
	x86_cpu_to_apicid[cpu] = -1;
	cpu_clear(cpu, cpu_present_map);
	num_processors--;

	return (0);
}