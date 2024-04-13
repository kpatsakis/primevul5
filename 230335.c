static void hci_hardware_error_evt(struct hci_dev *hdev, struct sk_buff *skb)
{
	struct hci_ev_hardware_error *ev = (void *) skb->data;

	hdev->hw_error_code = ev->code;

	queue_work(hdev->req_workqueue, &hdev->error_reset);
}