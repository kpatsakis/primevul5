void kvm_vcpu_block(struct kvm_vcpu *vcpu)
{
	ktime_t start, cur;
	DECLARE_SWAITQUEUE(wait);
	bool waited = false;
	u64 block_ns;

	kvm_arch_vcpu_blocking(vcpu);

	start = cur = ktime_get();
	if (vcpu->halt_poll_ns && !kvm_arch_no_poll(vcpu)) {
		ktime_t stop = ktime_add_ns(ktime_get(), vcpu->halt_poll_ns);

		++vcpu->stat.halt_attempted_poll;
		do {
			/*
			 * This sets KVM_REQ_UNHALT if an interrupt
			 * arrives.
			 */
			if (kvm_vcpu_check_block(vcpu) < 0) {
				++vcpu->stat.halt_successful_poll;
				if (!vcpu_valid_wakeup(vcpu))
					++vcpu->stat.halt_poll_invalid;
				goto out;
			}
			cur = ktime_get();
		} while (single_task_running() && ktime_before(cur, stop));
	}

	for (;;) {
		prepare_to_swait_exclusive(&vcpu->wq, &wait, TASK_INTERRUPTIBLE);

		if (kvm_vcpu_check_block(vcpu) < 0)
			break;

		waited = true;
		schedule();
	}

	finish_swait(&vcpu->wq, &wait);
	cur = ktime_get();
out:
	kvm_arch_vcpu_unblocking(vcpu);
	block_ns = ktime_to_ns(cur) - ktime_to_ns(start);

	if (!kvm_arch_no_poll(vcpu)) {
		if (!vcpu_valid_wakeup(vcpu)) {
			shrink_halt_poll_ns(vcpu);
		} else if (halt_poll_ns) {
			if (block_ns <= vcpu->halt_poll_ns)
				;
			/* we had a long block, shrink polling */
			else if (vcpu->halt_poll_ns && block_ns > halt_poll_ns)
				shrink_halt_poll_ns(vcpu);
			/* we had a short halt and our poll time is too small */
			else if (vcpu->halt_poll_ns < halt_poll_ns &&
				block_ns < halt_poll_ns)
				grow_halt_poll_ns(vcpu);
		} else {
			vcpu->halt_poll_ns = 0;
		}
	}

	trace_kvm_vcpu_wakeup(block_ns, waited, vcpu_valid_wakeup(vcpu));
	kvm_arch_vcpu_block_finish(vcpu);
}