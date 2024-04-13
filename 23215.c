static int __init efi_memreserve_root_init(void)
{
	if (efi_memreserve_root)
		return 0;
	if (efi_memreserve_map_root())
		efi_memreserve_root = (void *)ULONG_MAX;
	return 0;
}