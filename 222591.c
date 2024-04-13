static void kvm_s390_vcpu_crypto_setup(struct kvm_vcpu *vcpu)
{
	/*
	 * If the AP instructions are not being interpreted and the MSAX3
	 * facility is not configured for the guest, there is nothing to set up.
	 */
	if (!vcpu->kvm->arch.crypto.apie && !test_kvm_facility(vcpu->kvm, 76))
		return;

	vcpu->arch.sie_block->crycbd = vcpu->kvm->arch.crypto.crycbd;
	vcpu->arch.sie_block->ecb3 &= ~(ECB3_AES | ECB3_DEA);
	vcpu->arch.sie_block->eca &= ~ECA_APIE;
	vcpu->arch.sie_block->ecd &= ~ECD_ECC;

	if (vcpu->kvm->arch.crypto.apie)
		vcpu->arch.sie_block->eca |= ECA_APIE;

	/* Set up protected key support */
	if (vcpu->kvm->arch.crypto.aes_kw) {
		vcpu->arch.sie_block->ecb3 |= ECB3_AES;
		/* ecc is also wrapped with AES key */
		if (kvm_has_pckmo_ecc(vcpu->kvm))
			vcpu->arch.sie_block->ecd |= ECD_ECC;
	}

	if (vcpu->kvm->arch.crypto.dea_kw)
		vcpu->arch.sie_block->ecb3 |= ECB3_DEA;
}