static u64 svm_compute_tsc_offset(struct kvm_vcpu *vcpu, u64 target_tsc)
{
	u64 tsc;

	tsc = svm_scale_tsc(vcpu, native_read_tsc());

	return target_tsc - tsc;
}