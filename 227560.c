bool kvm_vcpu_wake_up(struct kvm_vcpu *vcpu)
{
	struct rcuwait *waitp;

	waitp = kvm_arch_vcpu_get_wait(vcpu);
	if (rcuwait_wake_up(waitp)) {
		WRITE_ONCE(vcpu->ready, true);
		++vcpu->stat.halt_wakeup;
		return true;
	}

	return false;
}