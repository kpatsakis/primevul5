void kvm_arch_async_page_not_present(struct kvm_vcpu *vcpu,
				     struct kvm_async_pf *work)
{
	trace_kvm_s390_pfault_init(vcpu, work->arch.pfault_token);
	__kvm_inject_pfault_token(vcpu, true, work->arch.pfault_token);
}