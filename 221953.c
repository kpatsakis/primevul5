static void syndbg_exit(struct kvm_vcpu *vcpu, u32 msr)
{
	struct kvm_hv_syndbg *syndbg = to_hv_syndbg(vcpu);
	struct kvm_vcpu_hv *hv_vcpu = to_hv_vcpu(vcpu);

	hv_vcpu->exit.type = KVM_EXIT_HYPERV_SYNDBG;
	hv_vcpu->exit.u.syndbg.msr = msr;
	hv_vcpu->exit.u.syndbg.control = syndbg->control.control;
	hv_vcpu->exit.u.syndbg.send_page = syndbg->control.send_page;
	hv_vcpu->exit.u.syndbg.recv_page = syndbg->control.recv_page;
	hv_vcpu->exit.u.syndbg.pending_page = syndbg->control.pending_page;
	vcpu->arch.complete_userspace_io =
			kvm_hv_syndbg_complete_userspace;

	kvm_make_request(KVM_REQ_HV_EXIT, vcpu);
}