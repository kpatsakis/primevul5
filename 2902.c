static inline void v4l2l_get_timestamp(struct v4l2_buffer *b)
{
	/* ktime_get_ts is considered deprecated, so use ktime_get_ts64 if possible */
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 17, 0)
	struct timespec ts;
	ktime_get_ts(&ts);
#else
	struct timespec64 ts;
	ktime_get_ts64(&ts);
#endif

	b->timestamp.tv_sec = ts.tv_sec;
	b->timestamp.tv_usec = (ts.tv_nsec / NSEC_PER_USEC);
}