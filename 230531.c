static void svm_vcpu_unblocking(struct kvm_vcpu *vcpu)
{
	if (kvm_check_request(KVM_REQ_APICV_UPDATE, vcpu))
		kvm_vcpu_update_apicv(vcpu);
	avic_set_running(vcpu, true);
}