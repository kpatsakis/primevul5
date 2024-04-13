void hci_req_purge(struct hci_request *req)
{
	skb_queue_purge(&req->cmd_q);
}