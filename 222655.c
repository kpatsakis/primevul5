void kvm_s390_vcpu_crypto_reset_all(struct kvm *kvm)
{
	struct kvm_vcpu *vcpu;
	int i;

	kvm_s390_vcpu_block_all(kvm);

	kvm_for_each_vcpu(i, vcpu, kvm) {
		kvm_s390_vcpu_crypto_setup(vcpu);
		/* recreate the shadow crycb by leaving the VSIE handler */
		kvm_s390_sync_request(KVM_REQ_VSIE_RESTART, vcpu);
	}

	kvm_s390_vcpu_unblock_all(kvm);
}