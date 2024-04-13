bool kvm_make_vcpus_request_mask(struct kvm *kvm, unsigned int req,
				 struct kvm_vcpu *except,
				 unsigned long *vcpu_bitmap, cpumask_var_t tmp)
{
	int i, cpu, me;
	struct kvm_vcpu *vcpu;
	bool called;

	me = get_cpu();

	kvm_for_each_vcpu(i, vcpu, kvm) {
		if ((vcpu_bitmap && !test_bit(i, vcpu_bitmap)) ||
		    vcpu == except)
			continue;

		kvm_make_request(req, vcpu);
		cpu = vcpu->cpu;

		if (!(req & KVM_REQUEST_NO_WAKEUP) && kvm_vcpu_wake_up(vcpu))
			continue;

		if (tmp != NULL && cpu != -1 && cpu != me &&
		    kvm_request_needs_ipi(vcpu, req))
			__cpumask_set_cpu(cpu, tmp);
	}

	called = kvm_kick_many_cpus(tmp, !!(req & KVM_REQUEST_WAIT));
	put_cpu();

	return called;
}