BOOL rdp_send_demand_active(rdpRdp* rdp)
{
	wStream* s = rdp_send_stream_pdu_init(rdp);
	BOOL status;

	if (!s)
		return FALSE;

	rdp->settings->ShareId = 0x10000 + rdp->mcs->userId;
	status = rdp_write_demand_active(s, rdp->settings) &&
	         rdp_send_pdu(rdp, s, PDU_TYPE_DEMAND_ACTIVE, rdp->mcs->userId);
	Stream_Release(s);
	return status;
}