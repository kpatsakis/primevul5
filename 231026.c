static void svm_set_tsc_khz(struct kvm_vcpu *vcpu, u32 user_tsc_khz, bool scale)
{
	struct vcpu_svm *svm = to_svm(vcpu);
	u64 ratio;
	u64 khz;

	/* Guest TSC same frequency as host TSC? */
	if (!scale) {
		svm->tsc_ratio = TSC_RATIO_DEFAULT;
		return;
	}

	/* TSC scaling supported? */
	if (!boot_cpu_has(X86_FEATURE_TSCRATEMSR)) {
		if (user_tsc_khz > tsc_khz) {
			vcpu->arch.tsc_catchup = 1;
			vcpu->arch.tsc_always_catchup = 1;
		} else
			WARN(1, "user requested TSC rate below hardware speed\n");
		return;
	}

	khz = user_tsc_khz;

	/* TSC scaling required  - calculate ratio */
	ratio = khz << 32;
	do_div(ratio, tsc_khz);

	if (ratio == 0 || ratio & TSC_RATIO_RSVD) {
		WARN_ONCE(1, "Invalid TSC ratio - virtual-tsc-khz=%u\n",
				user_tsc_khz);
		return;
	}
	svm->tsc_ratio             = ratio;
}