static int register_update_efi_random_seed(void)
{
	if (efi.rng_seed == EFI_INVALID_TABLE_ADDR)
		return 0;
	return register_reboot_notifier(&efi_random_seed_nb);
}