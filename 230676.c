static __init void svm_adjust_mmio_mask(void)
{
	unsigned int enc_bit, mask_bit;
	u64 msr, mask;

	/* If there is no memory encryption support, use existing mask */
	if (cpuid_eax(0x80000000) < 0x8000001f)
		return;

	/* If memory encryption is not enabled, use existing mask */
	rdmsrl(MSR_K8_SYSCFG, msr);
	if (!(msr & MSR_K8_SYSCFG_MEM_ENCRYPT))
		return;

	enc_bit = cpuid_ebx(0x8000001f) & 0x3f;
	mask_bit = boot_cpu_data.x86_phys_bits;

	/* Increment the mask bit if it is the same as the encryption bit */
	if (enc_bit == mask_bit)
		mask_bit++;

	/*
	 * If the mask bit location is below 52, then some bits above the
	 * physical addressing limit will always be reserved, so use the
	 * rsvd_bits() function to generate the mask. This mask, along with
	 * the present bit, will be used to generate a page fault with
	 * PFER.RSV = 1.
	 *
	 * If the mask bit location is 52 (or above), then clear the mask.
	 */
	mask = (mask_bit < 52) ? rsvd_bits(mask_bit, 51) | PT_PRESENT_MASK : 0;

	kvm_mmu_set_mmio_spte_mask(mask, mask, PT_WRITABLE_MASK | PT_USER_MASK);
}