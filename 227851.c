BOOL rdp_send_confirm_active(rdpRdp* rdp)
{
	wStream* s = rdp_send_stream_pdu_init(rdp);
	BOOL status;

	if (!s)
		return FALSE;

	status = rdp_write_confirm_active(s, rdp->settings) &&
	         rdp_send_pdu(rdp, s, PDU_TYPE_CONFIRM_ACTIVE, rdp->mcs->userId);
	Stream_Release(s);
	return status;
}