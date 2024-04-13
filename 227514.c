update_halt_poll_stats(struct kvm_vcpu *vcpu, u64 poll_ns, bool waited)
{
	if (waited)
		vcpu->stat.halt_poll_fail_ns += poll_ns;
	else
		vcpu->stat.halt_poll_success_ns += poll_ns;
}