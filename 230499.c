static int gp_interception(struct vcpu_svm *svm)
{
	struct kvm_vcpu *vcpu = &svm->vcpu;
	u32 error_code = svm->vmcb->control.exit_info_1;

	WARN_ON_ONCE(!enable_vmware_backdoor);

	/*
	 * VMware backdoor emulation on #GP interception only handles IN{S},
	 * OUT{S}, and RDPMC, none of which generate a non-zero error code.
	 */
	if (error_code) {
		kvm_queue_exception_e(vcpu, GP_VECTOR, error_code);
		return 1;
	}
	return kvm_emulate_instruction(vcpu, EMULTYPE_VMWARE_GP);
}