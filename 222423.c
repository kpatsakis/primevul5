static bool ibs_enabled(struct kvm_vcpu *vcpu)
{
	return kvm_s390_test_cpuflags(vcpu, CPUSTAT_IBS);
}