static void __enable_ibs_on_vcpu(struct kvm_vcpu *vcpu)
{
	if (!sclp.has_ibs)
		return;
	kvm_check_request(KVM_REQ_DISABLE_IBS, vcpu);
	kvm_s390_sync_request(KVM_REQ_ENABLE_IBS, vcpu);
}