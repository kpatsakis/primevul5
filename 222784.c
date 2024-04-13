void kvm_s390_set_cpu_timer(struct kvm_vcpu *vcpu, __u64 cputm)
{
	preempt_disable(); /* protect from TOD sync and vcpu_load/put */
	raw_write_seqcount_begin(&vcpu->arch.cputm_seqcount);
	if (vcpu->arch.cputm_enabled)
		vcpu->arch.cputm_start = get_tod_clock_fast();
	vcpu->arch.sie_block->cputm = cputm;
	raw_write_seqcount_end(&vcpu->arch.cputm_seqcount);
	preempt_enable();
}