static inline u32 tcp_cookie_time(void)
{
	u64 val = get_jiffies_64();

	do_div(val, TCP_SYNCOOKIE_PERIOD);
	return val;
}