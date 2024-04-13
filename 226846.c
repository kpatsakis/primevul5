static void sctp_wake_up_waiters(struct sock *sk,
				 struct sctp_association *asoc)
{
	struct sctp_association *tmp = asoc;

	/* We do accounting for the sndbuf space per association,
	 * so we only need to wake our own association.
	 */
	if (asoc->ep->sndbuf_policy)
		return __sctp_write_space(asoc);

	/* If association goes down and is just flushing its
	 * outq, then just normally notify others.
	 */
	if (asoc->base.dead)
		return sctp_write_space(sk);

	/* Accounting for the sndbuf space is per socket, so we
	 * need to wake up others, try to be fair and in case of
	 * other associations, let them have a go first instead
	 * of just doing a sctp_write_space() call.
	 *
	 * Note that we reach sctp_wake_up_waiters() only when
	 * associations free up queued chunks, thus we are under
	 * lock and the list of associations on a socket is
	 * guaranteed not to change.
	 */
	for (tmp = list_next_entry(tmp, asocs); 1;
	     tmp = list_next_entry(tmp, asocs)) {
		/* Manually skip the head element. */
		if (&tmp->asocs == &((sctp_sk(sk))->ep->asocs))
			continue;
		/* Wake up association. */
		__sctp_write_space(tmp);
		/* We've reached the end. */
		if (tmp == asoc)
			break;
	}
}