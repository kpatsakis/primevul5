void __init efi_mem_reserve(phys_addr_t addr, u64 size)
{
	if (!memblock_is_region_reserved(addr, size))
		memblock_reserve(addr, size);

	/*
	 * Some architectures (x86) reserve all boot services ranges
	 * until efi_free_boot_services() because of buggy firmware
	 * implementations. This means the above memblock_reserve() is
	 * superfluous on x86 and instead what it needs to do is
	 * ensure the @start, @size is not freed.
	 */
	efi_arch_mem_reserve(addr, size);
}