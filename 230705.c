static int rsm_interception(struct vcpu_svm *svm)
{
	return kvm_emulate_instruction_from_buffer(&svm->vcpu, rsm_ins_bytes, 2);
}