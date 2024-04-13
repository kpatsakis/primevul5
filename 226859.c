static inline __u32 sctp_spp_sackdelay_disable(__u32 param_flags)
{
	return (param_flags & ~SPP_SACKDELAY) | SPP_SACKDELAY_DISABLE;
}