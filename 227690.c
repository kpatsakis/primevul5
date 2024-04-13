static void ca8210_hw_setup(struct ieee802154_hw *ca8210_hw)
{
	/* Support channels 11-26 */
	ca8210_hw->phy->supported.channels[0] = CA8210_VALID_CHANNELS;
	ca8210_hw->phy->supported.tx_powers_size = CA8210_MAX_TX_POWERS;
	ca8210_hw->phy->supported.tx_powers = ca8210_tx_powers;
	ca8210_hw->phy->supported.cca_ed_levels_size = CA8210_MAX_ED_LEVELS;
	ca8210_hw->phy->supported.cca_ed_levels = ca8210_ed_levels;
	ca8210_hw->phy->current_channel = 18;
	ca8210_hw->phy->current_page = 0;
	ca8210_hw->phy->transmit_power = 800;
	ca8210_hw->phy->cca.mode = NL802154_CCA_ENERGY_CARRIER;
	ca8210_hw->phy->cca.opt = NL802154_CCA_OPT_ENERGY_CARRIER_AND;
	ca8210_hw->phy->cca_ed_level = -9800;
	ca8210_hw->phy->symbol_duration = 16;
	ca8210_hw->phy->lifs_period = 40;
	ca8210_hw->phy->sifs_period = 12;
	ca8210_hw->flags =
		IEEE802154_HW_AFILT |
		IEEE802154_HW_OMIT_CKSUM |
		IEEE802154_HW_FRAME_RETRIES |
		IEEE802154_HW_PROMISCUOUS |
		IEEE802154_HW_CSMA_PARAMS;
	ca8210_hw->phy->flags =
		WPAN_PHY_FLAG_TXPOWER |
		WPAN_PHY_FLAG_CCA_ED_LEVEL |
		WPAN_PHY_FLAG_CCA_MODE;
}