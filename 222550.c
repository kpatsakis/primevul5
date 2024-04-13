static unsigned long kvm_s390_fac_size(void)
{
	BUILD_BUG_ON(SIZE_INTERNAL > S390_ARCH_FAC_MASK_SIZE_U64);
	BUILD_BUG_ON(SIZE_INTERNAL > S390_ARCH_FAC_LIST_SIZE_U64);
	BUILD_BUG_ON(SIZE_INTERNAL * sizeof(unsigned long) >
		sizeof(S390_lowcore.stfle_fac_list));

	return SIZE_INTERNAL;
}