void kvm_s390_sync_request(int req, struct kvm_vcpu *vcpu)
{
	kvm_make_request(req, vcpu);
	kvm_s390_vcpu_request(vcpu);
}