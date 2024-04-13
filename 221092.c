static inline struct timespec64 ep_set_mstimeout(long ms)
{
	struct timespec64 now, ts = {
		.tv_sec = ms / MSEC_PER_SEC,
		.tv_nsec = NSEC_PER_MSEC * (ms % MSEC_PER_SEC),
	};

	ktime_get_ts64(&now);
	return timespec64_add_safe(now, ts);
}