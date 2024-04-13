void tcp_update_metrics(struct sock *sk)
{
	struct tcp_sock *tp = tcp_sk(sk);
	struct dst_entry *dst = __sk_dst_get(sk);

	if (sysctl_tcp_nometrics_save)
		return;

	dst_confirm(dst);

	if (dst && (dst->flags & DST_HOST)) {
		const struct inet_connection_sock *icsk = inet_csk(sk);
		int m;
		unsigned long rtt;

		if (icsk->icsk_backoff || !tp->srtt) {
			/* This session failed to estimate rtt. Why?
			 * Probably, no packets returned in time.
			 * Reset our results.
			 */
			if (!(dst_metric_locked(dst, RTAX_RTT)))
				dst_metric_set(dst, RTAX_RTT, 0);
			return;
		}

		rtt = dst_metric_rtt(dst, RTAX_RTT);
		m = rtt - tp->srtt;

		/* If newly calculated rtt larger than stored one,
		 * store new one. Otherwise, use EWMA. Remember,
		 * rtt overestimation is always better than underestimation.
		 */
		if (!(dst_metric_locked(dst, RTAX_RTT))) {
			if (m <= 0)
				set_dst_metric_rtt(dst, RTAX_RTT, tp->srtt);
			else
				set_dst_metric_rtt(dst, RTAX_RTT, rtt - (m >> 3));
		}

		if (!(dst_metric_locked(dst, RTAX_RTTVAR))) {
			unsigned long var;
			if (m < 0)
				m = -m;

			/* Scale deviation to rttvar fixed point */
			m >>= 1;
			if (m < tp->mdev)
				m = tp->mdev;

			var = dst_metric_rtt(dst, RTAX_RTTVAR);
			if (m >= var)
				var = m;
			else
				var -= (var - m) >> 2;

			set_dst_metric_rtt(dst, RTAX_RTTVAR, var);
		}

		if (tcp_in_initial_slowstart(tp)) {
			/* Slow start still did not finish. */
			if (dst_metric(dst, RTAX_SSTHRESH) &&
			    !dst_metric_locked(dst, RTAX_SSTHRESH) &&
			    (tp->snd_cwnd >> 1) > dst_metric(dst, RTAX_SSTHRESH))
				dst_metric_set(dst, RTAX_SSTHRESH, tp->snd_cwnd >> 1);
			if (!dst_metric_locked(dst, RTAX_CWND) &&
			    tp->snd_cwnd > dst_metric(dst, RTAX_CWND))
				dst_metric_set(dst, RTAX_CWND, tp->snd_cwnd);
		} else if (tp->snd_cwnd > tp->snd_ssthresh &&
			   icsk->icsk_ca_state == TCP_CA_Open) {
			/* Cong. avoidance phase, cwnd is reliable. */
			if (!dst_metric_locked(dst, RTAX_SSTHRESH))
				dst_metric_set(dst, RTAX_SSTHRESH,
					       max(tp->snd_cwnd >> 1, tp->snd_ssthresh));
			if (!dst_metric_locked(dst, RTAX_CWND))
				dst_metric_set(dst, RTAX_CWND,
					       (dst_metric(dst, RTAX_CWND) +
						tp->snd_cwnd) >> 1);
		} else {
			/* Else slow start did not finish, cwnd is non-sense,
			   ssthresh may be also invalid.
			 */
			if (!dst_metric_locked(dst, RTAX_CWND))
				dst_metric_set(dst, RTAX_CWND,
					       (dst_metric(dst, RTAX_CWND) +
						tp->snd_ssthresh) >> 1);
			if (dst_metric(dst, RTAX_SSTHRESH) &&
			    !dst_metric_locked(dst, RTAX_SSTHRESH) &&
			    tp->snd_ssthresh > dst_metric(dst, RTAX_SSTHRESH))
				dst_metric_set(dst, RTAX_SSTHRESH, tp->snd_ssthresh);
		}

		if (!dst_metric_locked(dst, RTAX_REORDERING)) {
			if (dst_metric(dst, RTAX_REORDERING) < tp->reordering &&
			    tp->reordering != sysctl_tcp_reordering)
				dst_metric_set(dst, RTAX_REORDERING, tp->reordering);
		}
	}
}