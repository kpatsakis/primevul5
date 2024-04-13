static int xsetbv_interception(struct vcpu_svm *svm)
{
	u64 new_bv = kvm_read_edx_eax(&svm->vcpu);
	u32 index = kvm_register_read(&svm->vcpu, VCPU_REGS_RCX);

	if (kvm_set_xcr(&svm->vcpu, index, new_bv) == 0) {
		svm->next_rip = kvm_rip_read(&svm->vcpu) + 3;
		skip_emulated_instruction(&svm->vcpu);
	}

	return 1;
}