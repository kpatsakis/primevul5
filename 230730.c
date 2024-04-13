static int svm_pre_leave_smm(struct kvm_vcpu *vcpu, const char *smstate)
{
	struct vcpu_svm *svm = to_svm(vcpu);
	struct vmcb *nested_vmcb;
	struct kvm_host_map map;
	u64 guest;
	u64 vmcb;

	guest = GET_SMSTATE(u64, smstate, 0x7ed8);
	vmcb = GET_SMSTATE(u64, smstate, 0x7ee0);

	if (guest) {
		if (kvm_vcpu_map(&svm->vcpu, gpa_to_gfn(vmcb), &map) == -EINVAL)
			return 1;
		nested_vmcb = map.hva;
		enter_svm_guest_mode(svm, vmcb, nested_vmcb, &map);
	}
	return 0;
}