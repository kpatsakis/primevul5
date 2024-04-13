static void kvm_s390_set_crycb_format(struct kvm *kvm)
{
	kvm->arch.crypto.crycbd = (__u32)(unsigned long) kvm->arch.crypto.crycb;

	/* Clear the CRYCB format bits - i.e., set format 0 by default */
	kvm->arch.crypto.crycbd &= ~(CRYCB_FORMAT_MASK);

	/* Check whether MSAX3 is installed */
	if (!test_kvm_facility(kvm, 76))
		return;

	if (kvm_s390_apxa_installed())
		kvm->arch.crypto.crycbd |= CRYCB_FORMAT2;
	else
		kvm->arch.crypto.crycbd |= CRYCB_FORMAT1;
}