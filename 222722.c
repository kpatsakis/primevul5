void kvm_arch_crypto_clear_masks(struct kvm *kvm)
{
	mutex_lock(&kvm->lock);
	kvm_s390_vcpu_block_all(kvm);

	memset(&kvm->arch.crypto.crycb->apcb0, 0,
	       sizeof(kvm->arch.crypto.crycb->apcb0));
	memset(&kvm->arch.crypto.crycb->apcb1, 0,
	       sizeof(kvm->arch.crypto.crycb->apcb1));

	VM_EVENT(kvm, 3, "%s", "CLR CRYCB:");
	/* recreate the shadow crycb for each vcpu */
	kvm_s390_sync_request_broadcast(kvm, KVM_REQ_VSIE_RESTART);
	kvm_s390_vcpu_unblock_all(kvm);
	mutex_unlock(&kvm->lock);
}