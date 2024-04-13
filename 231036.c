static u64 svm_read_l1_tsc(struct kvm_vcpu *vcpu, u64 host_tsc)
{
	struct vmcb *vmcb = get_host_vmcb(to_svm(vcpu));
	return vmcb->control.tsc_offset +
		svm_scale_tsc(vcpu, host_tsc);
}