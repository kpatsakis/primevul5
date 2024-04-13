static void del_from_white_list(struct hci_request *req, bdaddr_t *bdaddr,
				u8 bdaddr_type)
{
	struct hci_cp_le_del_from_white_list cp;

	cp.bdaddr_type = bdaddr_type;
	bacpy(&cp.bdaddr, bdaddr);

	bt_dev_dbg(req->hdev, "Remove %pMR (0x%x) from whitelist", &cp.bdaddr,
		   cp.bdaddr_type);
	hci_req_add(req, HCI_OP_LE_DEL_FROM_WHITE_LIST, sizeof(cp), &cp);

	if (use_ll_privacy(req->hdev) &&
	    hci_dev_test_flag(req->hdev, HCI_ENABLE_LL_PRIVACY)) {
		struct smp_irk *irk;

		irk = hci_find_irk_by_addr(req->hdev, bdaddr, bdaddr_type);
		if (irk) {
			struct hci_cp_le_del_from_resolv_list cp;

			cp.bdaddr_type = bdaddr_type;
			bacpy(&cp.bdaddr, bdaddr);

			hci_req_add(req, HCI_OP_LE_DEL_FROM_RESOLV_LIST,
				    sizeof(cp), &cp);
		}
	}
}