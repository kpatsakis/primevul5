static void hci_cc_read_data_block_size(struct hci_dev *hdev,
					struct sk_buff *skb)
{
	struct hci_rp_read_data_block_size *rp = (void *) skb->data;

	BT_DBG("%s status 0x%2.2x", hdev->name, rp->status);

	if (rp->status)
		return;

	hdev->block_mtu = __le16_to_cpu(rp->max_acl_len);
	hdev->block_len = __le16_to_cpu(rp->block_len);
	hdev->num_blocks = __le16_to_cpu(rp->num_blocks);

	hdev->block_cnt = hdev->num_blocks;

	BT_DBG("%s blk mtu %d cnt %d len %d", hdev->name, hdev->block_mtu,
	       hdev->block_cnt, hdev->block_len);
}