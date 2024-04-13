static void svm_sched_in(struct kvm_vcpu *vcpu, int cpu)
{
	if (pause_filter_thresh)
		shrink_ple_window(vcpu);
}