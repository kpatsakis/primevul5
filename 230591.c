static int xsetbv_interception(struct vcpu_svm *svm)
{
	u64 new_bv = kvm_read_edx_eax(&svm->vcpu);
	u32 index = kvm_rcx_read(&svm->vcpu);

	if (kvm_set_xcr(&svm->vcpu, index, new_bv) == 0) {
		return kvm_skip_emulated_instruction(&svm->vcpu);
	}

	return 1;
}