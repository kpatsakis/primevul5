static inline bool tcp_ca_dst_locked(const struct dst_entry *dst)
{
	return dst_metric_locked(dst, RTAX_CC_ALGO);
}