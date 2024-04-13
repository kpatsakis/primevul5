static u64 svm_scale_tsc(struct kvm_vcpu *vcpu, u64 tsc)
{
	struct vcpu_svm *svm = to_svm(vcpu);
	u64 _tsc = tsc;

	if (svm->tsc_ratio != TSC_RATIO_DEFAULT)
		_tsc = __scale_tsc(svm->tsc_ratio, tsc);

	return _tsc;
}