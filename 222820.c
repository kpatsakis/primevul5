static void kvm_s390_crypto_init(struct kvm *kvm)
{
	kvm->arch.crypto.crycb = &kvm->arch.sie_page2->crycb;
	kvm_s390_set_crycb_format(kvm);

	if (!test_kvm_facility(kvm, 76))
		return;

	/* Enable AES/DEA protected key functions by default */
	kvm->arch.crypto.aes_kw = 1;
	kvm->arch.crypto.dea_kw = 1;
	get_random_bytes(kvm->arch.crypto.crycb->aes_wrapping_key_mask,
			 sizeof(kvm->arch.crypto.crycb->aes_wrapping_key_mask));
	get_random_bytes(kvm->arch.crypto.crycb->dea_wrapping_key_mask,
			 sizeof(kvm->arch.crypto.crycb->dea_wrapping_key_mask));
}