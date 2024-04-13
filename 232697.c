static int rsi_map_region_code(enum nl80211_dfs_regions region_code)
{
	switch (region_code) {
	case NL80211_DFS_FCC:
		return RSI_REGION_FCC;
	case NL80211_DFS_ETSI:
		return RSI_REGION_ETSI;
	case NL80211_DFS_JP:
		return RSI_REGION_TELEC;
	case NL80211_DFS_UNSET:
		return RSI_REGION_WORLD;
	}
	return RSI_REGION_WORLD;
}