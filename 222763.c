static void kvm_s390_sync_request_broadcast(struct kvm *kvm, int req)
{
	int cx;
	struct kvm_vcpu *vcpu;

	kvm_for_each_vcpu(cx, vcpu, kvm)
		kvm_s390_sync_request(req, vcpu);
}