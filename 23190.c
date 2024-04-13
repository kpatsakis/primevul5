static int __init efi_memreserve_map_root(void)
{
	if (efi.mem_reserve == EFI_INVALID_TABLE_ADDR)
		return -ENODEV;

	efi_memreserve_root = memremap(efi.mem_reserve,
				       sizeof(*efi_memreserve_root),
				       MEMREMAP_WB);
	if (WARN_ON_ONCE(!efi_memreserve_root))
		return -ENOMEM;
	return 0;
}