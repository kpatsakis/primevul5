static void mcba_usb_process_can(struct mcba_priv *priv,
				 struct mcba_usb_msg_can *msg)
{
	struct can_frame *cf;
	struct sk_buff *skb;
	struct net_device_stats *stats = &priv->netdev->stats;
	u16 sid;

	skb = alloc_can_skb(priv->netdev, &cf);
	if (!skb)
		return;

	sid = get_unaligned_be16(&msg->sid);

	if (sid & MCBA_SIDL_EXID_MASK) {
		/* SIDH    | SIDL                 | EIDH   | EIDL
		 * 28 - 21 | 20 19 18 x x x 17 16 | 15 - 8 | 7 - 0
		 */
		cf->can_id = CAN_EFF_FLAG;

		/* store 28-18 bits */
		cf->can_id |= (sid & 0xffe0) << 13;
		/* store 17-16 bits */
		cf->can_id |= (sid & 3) << 16;
		/* store 15-0 bits */
		cf->can_id |= get_unaligned_be16(&msg->eid);
	} else {
		/* SIDH   | SIDL
		 * 10 - 3 | 2 1 0 x x x x x
		 */
		cf->can_id = (sid & 0xffe0) >> 5;
	}

	if (msg->dlc & MCBA_DLC_RTR_MASK)
		cf->can_id |= CAN_RTR_FLAG;

	cf->can_dlc = get_can_dlc(msg->dlc & MCBA_DLC_MASK);

	memcpy(cf->data, msg->data, cf->can_dlc);

	stats->rx_packets++;
	stats->rx_bytes += cf->can_dlc;

	can_led_event(priv->netdev, CAN_LED_EVENT_RX);
	netif_rx(skb);
}