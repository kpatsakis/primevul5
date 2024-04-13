static int rsi_map_intf_mode(enum nl80211_iftype vif_type)
{
	switch (vif_type) {
	case NL80211_IFTYPE_STATION:
		return RSI_OPMODE_STA;
	case NL80211_IFTYPE_AP:
		return RSI_OPMODE_AP;
	case NL80211_IFTYPE_P2P_DEVICE:
		return RSI_OPMODE_P2P_CLIENT;
	case NL80211_IFTYPE_P2P_CLIENT:
		return RSI_OPMODE_P2P_CLIENT;
	case NL80211_IFTYPE_P2P_GO:
		return RSI_OPMODE_P2P_GO;
	default:
		return RSI_OPMODE_UNSUPPORTED;
	}
}