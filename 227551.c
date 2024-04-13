bool kvm_make_all_cpus_request_except(struct kvm *kvm, unsigned int req,
				      struct kvm_vcpu *except)
{
	cpumask_var_t cpus;
	bool called;

	zalloc_cpumask_var(&cpus, GFP_ATOMIC);

	called = kvm_make_vcpus_request_mask(kvm, req, except, NULL, cpus);

	free_cpumask_var(cpus);
	return called;
}