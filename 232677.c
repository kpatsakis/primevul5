static void rsi_mac80211_conf_filter(struct ieee80211_hw *hw,
				     u32 changed_flags,
				     u32 *total_flags,
				     u64 multicast)
{
	/* Not doing much here as of now */
	*total_flags &= RSI_SUPP_FILTERS;
}