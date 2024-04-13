char *__acpi_map_table(unsigned long phys_addr, unsigned long size)
{
	if (!phys_addr || !size)
		return NULL;

	if (phys_addr+size <= (end_pfn_map << PAGE_SHIFT) + PAGE_SIZE)
		return __va(phys_addr);

	return NULL;
}