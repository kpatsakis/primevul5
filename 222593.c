bool kvm_vcpu_wake_up(struct kvm_vcpu *vcpu)
{
	struct swait_queue_head *wqp;

	wqp = kvm_arch_vcpu_wq(vcpu);
	if (swq_has_sleeper(wqp)) {
		swake_up_one(wqp);
		WRITE_ONCE(vcpu->ready, true);
		++vcpu->stat.halt_wakeup;
		return true;
	}

	return false;
}