static int update_efi_random_seed(struct notifier_block *nb,
				  unsigned long code, void *unused)
{
	struct linux_efi_random_seed *seed;
	u32 size = 0;

	if (!kexec_in_progress)
		return NOTIFY_DONE;

	seed = memremap(efi.rng_seed, sizeof(*seed), MEMREMAP_WB);
	if (seed != NULL) {
		size = min(seed->size, EFI_RANDOM_SEED_SIZE);
		memunmap(seed);
	} else {
		pr_err("Could not map UEFI random seed!\n");
	}
	if (size > 0) {
		seed = memremap(efi.rng_seed, sizeof(*seed) + size,
				MEMREMAP_WB);
		if (seed != NULL) {
			seed->size = size;
			get_random_bytes(seed->bits, seed->size);
			memunmap(seed);
		} else {
			pr_err("Could not map UEFI random seed!\n");
		}
	}
	return NOTIFY_DONE;
}