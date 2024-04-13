static __u32 __packet_set_timestamp(struct packet_sock *po, void *frame,
				    struct sk_buff *skb)
{
	union tpacket_uhdr h;
	struct timespec64 ts;
	__u32 ts_status;

	if (!(ts_status = tpacket_get_timestamp(skb, &ts, po->tp_tstamp)))
		return 0;

	h.raw = frame;
	/*
	 * versions 1 through 3 overflow the timestamps in y2106, since they
	 * all store the seconds in a 32-bit unsigned integer.
	 * If we create a version 4, that should have a 64-bit timestamp,
	 * either 64-bit seconds + 32-bit nanoseconds, or just 64-bit
	 * nanoseconds.
	 */
	switch (po->tp_version) {
	case TPACKET_V1:
		h.h1->tp_sec = ts.tv_sec;
		h.h1->tp_usec = ts.tv_nsec / NSEC_PER_USEC;
		break;
	case TPACKET_V2:
		h.h2->tp_sec = ts.tv_sec;
		h.h2->tp_nsec = ts.tv_nsec;
		break;
	case TPACKET_V3:
		h.h3->tp_sec = ts.tv_sec;
		h.h3->tp_nsec = ts.tv_nsec;
		break;
	default:
		WARN(1, "TPACKET version not supported.\n");
		BUG();
	}

	/* one flush is safe, as both fields always lie on the same cacheline */
	flush_dcache_page(pgv_to_page(&h.h1->tp_sec));
	smp_wmb();

	return ts_status;
}