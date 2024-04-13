static void hci_cc_set_ext_adv_param(struct hci_dev *hdev, struct sk_buff *skb)
{
	struct hci_rp_le_set_ext_adv_params *rp = (void *) skb->data;
	struct hci_cp_le_set_ext_adv_params *cp;
	struct adv_info *adv_instance;

	BT_DBG("%s status 0x%2.2x", hdev->name, rp->status);

	if (rp->status)
		return;

	cp = hci_sent_cmd_data(hdev, HCI_OP_LE_SET_EXT_ADV_PARAMS);
	if (!cp)
		return;

	hci_dev_lock(hdev);
	hdev->adv_addr_type = cp->own_addr_type;
	if (!hdev->cur_adv_instance) {
		/* Store in hdev for instance 0 */
		hdev->adv_tx_power = rp->tx_power;
	} else {
		adv_instance = hci_find_adv_instance(hdev,
						     hdev->cur_adv_instance);
		if (adv_instance)
			adv_instance->tx_power = rp->tx_power;
	}
	/* Update adv data as tx power is known now */
	hci_req_update_adv_data(hdev, hdev->cur_adv_instance);
	hci_dev_unlock(hdev);
}