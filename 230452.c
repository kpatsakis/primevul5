static void hci_cc_le_set_adv_set_random_addr(struct hci_dev *hdev,
                                              struct sk_buff *skb)
{
	__u8 status = *((__u8 *) skb->data);
	struct hci_cp_le_set_adv_set_rand_addr *cp;
	struct adv_info *adv_instance;

	if (status)
		return;

	cp = hci_sent_cmd_data(hdev, HCI_OP_LE_SET_ADV_SET_RAND_ADDR);
	if (!cp)
		return;

	hci_dev_lock(hdev);

	if (!hdev->cur_adv_instance) {
		/* Store in hdev for instance 0 (Set adv and Directed advs) */
		bacpy(&hdev->random_addr, &cp->bdaddr);
	} else {
		adv_instance = hci_find_adv_instance(hdev,
						     hdev->cur_adv_instance);
		if (adv_instance)
			bacpy(&adv_instance->random_addr, &cp->bdaddr);
	}

	hci_dev_unlock(hdev);
}