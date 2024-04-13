static void __stop_cpu_timer_accounting(struct kvm_vcpu *vcpu)
{
	WARN_ON_ONCE(vcpu->arch.cputm_start == 0);
	raw_write_seqcount_begin(&vcpu->arch.cputm_seqcount);
	vcpu->arch.sie_block->cputm -= get_tod_clock_fast() - vcpu->arch.cputm_start;
	vcpu->arch.cputm_start = 0;
	raw_write_seqcount_end(&vcpu->arch.cputm_seqcount);
}