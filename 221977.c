static void stimer_expiration(struct kvm_vcpu_hv_stimer *stimer)
{
	int r, direct = stimer->config.direct_mode;

	stimer->msg_pending = true;
	if (!direct)
		r = stimer_send_msg(stimer);
	else
		r = stimer_notify_direct(stimer);
	trace_kvm_hv_stimer_expiration(hv_stimer_to_vcpu(stimer)->vcpu_id,
				       stimer->index, direct, r);
	if (!r) {
		stimer->msg_pending = false;
		if (!(stimer->config.periodic))
			stimer->config.enable = 0;
	}
}