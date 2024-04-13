static u16 rsi_wow_map_triggers(struct rsi_common *common,
				struct cfg80211_wowlan *wowlan)
{
	u16 wow_triggers = 0;

	rsi_dbg(INFO_ZONE, "Mapping wowlan triggers\n");

	if (wowlan->any)
		wow_triggers |= RSI_WOW_ANY;
	if (wowlan->magic_pkt)
		wow_triggers |= RSI_WOW_MAGIC_PKT;
	if (wowlan->disconnect)
		wow_triggers |= RSI_WOW_DISCONNECT;
	if (wowlan->gtk_rekey_failure || wowlan->eap_identity_req ||
	    wowlan->four_way_handshake)
		wow_triggers |= RSI_WOW_GTK_REKEY;

	return wow_triggers;
}