static void grow_halt_poll_ns(struct kvm_vcpu *vcpu)
{
	unsigned int old, val, grow, grow_start;

	old = val = vcpu->halt_poll_ns;
	grow_start = READ_ONCE(halt_poll_ns_grow_start);
	grow = READ_ONCE(halt_poll_ns_grow);
	if (!grow)
		goto out;

	val *= grow;
	if (val < grow_start)
		val = grow_start;

	if (val > vcpu->kvm->max_halt_poll_ns)
		val = vcpu->kvm->max_halt_poll_ns;

	vcpu->halt_poll_ns = val;
out:
	trace_kvm_halt_poll_ns_grow(vcpu->vcpu_id, val, old);
}