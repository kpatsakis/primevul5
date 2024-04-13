static int add_to_white_list(struct hci_request *req,
			     struct hci_conn_params *params, u8 *num_entries,
			     bool allow_rpa)
{
	struct hci_cp_le_add_to_white_list cp;
	struct hci_dev *hdev = req->hdev;

	/* Already in white list */
	if (hci_bdaddr_list_lookup(&hdev->le_white_list, &params->addr,
				   params->addr_type))
		return 0;

	/* Select filter policy to accept all advertising */
	if (*num_entries >= hdev->le_white_list_size)
		return -1;

	/* White list can not be used with RPAs */
	if (!allow_rpa &&
	    !hci_dev_test_flag(hdev, HCI_ENABLE_LL_PRIVACY) &&
	    hci_find_irk_by_addr(hdev, &params->addr, params->addr_type)) {
		return -1;
	}

	/* During suspend, only wakeable devices can be in whitelist */
	if (hdev->suspended && !hci_conn_test_flag(HCI_CONN_FLAG_REMOTE_WAKEUP,
						   params->current_flags))
		return 0;

	*num_entries += 1;
	cp.bdaddr_type = params->addr_type;
	bacpy(&cp.bdaddr, &params->addr);

	bt_dev_dbg(hdev, "Add %pMR (0x%x) to whitelist", &cp.bdaddr,
		   cp.bdaddr_type);
	hci_req_add(req, HCI_OP_LE_ADD_TO_WHITE_LIST, sizeof(cp), &cp);

	if (use_ll_privacy(hdev) &&
	    hci_dev_test_flag(hdev, HCI_ENABLE_LL_PRIVACY)) {
		struct smp_irk *irk;

		irk = hci_find_irk_by_addr(hdev, &params->addr,
					   params->addr_type);
		if (irk) {
			struct hci_cp_le_add_to_resolv_list cp;

			cp.bdaddr_type = params->addr_type;
			bacpy(&cp.bdaddr, &params->addr);
			memcpy(cp.peer_irk, irk->val, 16);

			if (hci_dev_test_flag(hdev, HCI_PRIVACY))
				memcpy(cp.local_irk, hdev->irk, 16);
			else
				memset(cp.local_irk, 0, 16);

			hci_req_add(req, HCI_OP_LE_ADD_TO_RESOLV_LIST,
				    sizeof(cp), &cp);
		}
	}

	return 0;
}