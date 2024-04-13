void __hci_req_clear_ext_adv_sets(struct hci_request *req)
{
	hci_req_add(req, HCI_OP_LE_CLEAR_ADV_SETS, 0, NULL);
}