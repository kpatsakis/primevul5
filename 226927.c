static u32 convert_can2host_bitrate(struct mcba_usb_msg_ka_can *msg)
{
	const u32 bitrate = get_unaligned_be16(&msg->can_bitrate);

	if ((bitrate == 33) || (bitrate == 83))
		return bitrate * 1000 + 333;
	else
		return bitrate * 1000;
}