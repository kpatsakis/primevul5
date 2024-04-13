static void hci_le_meta_evt(struct hci_dev *hdev, struct sk_buff *skb)
{
	struct hci_ev_le_meta *le_ev = (void *) skb->data;

	skb_pull(skb, sizeof(*le_ev));

	switch (le_ev->subevent) {
	case HCI_EV_LE_CONN_COMPLETE:
		hci_le_conn_complete_evt(hdev, skb);
		break;

	case HCI_EV_LE_CONN_UPDATE_COMPLETE:
		hci_le_conn_update_complete_evt(hdev, skb);
		break;

	case HCI_EV_LE_ADVERTISING_REPORT:
		hci_le_adv_report_evt(hdev, skb);
		break;

	case HCI_EV_LE_REMOTE_FEAT_COMPLETE:
		hci_le_remote_feat_complete_evt(hdev, skb);
		break;

	case HCI_EV_LE_LTK_REQ:
		hci_le_ltk_request_evt(hdev, skb);
		break;

	case HCI_EV_LE_REMOTE_CONN_PARAM_REQ:
		hci_le_remote_conn_param_req_evt(hdev, skb);
		break;

	case HCI_EV_LE_DIRECT_ADV_REPORT:
		hci_le_direct_adv_report_evt(hdev, skb);
		break;

	case HCI_EV_LE_PHY_UPDATE_COMPLETE:
		hci_le_phy_update_evt(hdev, skb);
		break;

	case HCI_EV_LE_EXT_ADV_REPORT:
		hci_le_ext_adv_report_evt(hdev, skb);
		break;

	case HCI_EV_LE_ENHANCED_CONN_COMPLETE:
		hci_le_enh_conn_complete_evt(hdev, skb);
		break;

	case HCI_EV_LE_EXT_ADV_SET_TERM:
		hci_le_ext_adv_term_evt(hdev, skb);
		break;

	default:
		break;
	}
}