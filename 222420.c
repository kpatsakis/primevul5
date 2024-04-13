__u64 kvm_s390_get_cpu_timer(struct kvm_vcpu *vcpu)
{
	unsigned int seq;
	__u64 value;

	if (unlikely(!vcpu->arch.cputm_enabled))
		return vcpu->arch.sie_block->cputm;

	preempt_disable(); /* protect from TOD sync and vcpu_load/put */
	do {
		seq = raw_read_seqcount(&vcpu->arch.cputm_seqcount);
		/*
		 * If the writer would ever execute a read in the critical
		 * section, e.g. in irq context, we have a deadlock.
		 */
		WARN_ON_ONCE((seq & 1) && smp_processor_id() == vcpu->cpu);
		value = vcpu->arch.sie_block->cputm;
		/* if cputm_start is 0, accounting is being started/stopped */
		if (likely(vcpu->arch.cputm_start))
			value -= get_tod_clock_fast() - vcpu->arch.cputm_start;
	} while (read_seqcount_retry(&vcpu->arch.cputm_seqcount, seq & ~1));
	preempt_enable();
	return value;
}