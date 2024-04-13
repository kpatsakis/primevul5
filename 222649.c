bool kvm_arch_no_poll(struct kvm_vcpu *vcpu)
{
	/* do not poll with more than halt_poll_max_steal percent of steal time */
	if (S390_lowcore.avg_steal_timer * 100 / (TICK_USEC << 12) >=
	    halt_poll_max_steal) {
		vcpu->stat.halt_no_poll_steal++;
		return true;
	}
	return false;
}