static BOOL autodetect_recv_netchar_result(rdpRdp* rdp, wStream* s,
                                           AUTODETECT_REQ_PDU* autodetectReqPdu)
{
	BOOL success = TRUE;

	switch (autodetectReqPdu->requestType)
	{
		case 0x0840:

			/* baseRTT and averageRTT fields are present (bandwidth field is not) */
			if ((autodetectReqPdu->headerLength != 0x0E) || (Stream_GetRemainingLength(s) < 8))
				return FALSE;

			Stream_Read_UINT32(s, rdp->autodetect->netCharBaseRTT);    /* baseRTT (4 bytes) */
			Stream_Read_UINT32(s, rdp->autodetect->netCharAverageRTT); /* averageRTT (4 bytes) */
			break;

		case 0x0880:

			/* bandwidth and averageRTT fields are present (baseRTT field is not) */
			if ((autodetectReqPdu->headerLength != 0x0E) || (Stream_GetRemainingLength(s) < 8))
				return FALSE;

			Stream_Read_UINT32(s, rdp->autodetect->netCharBandwidth);  /* bandwidth (4 bytes) */
			Stream_Read_UINT32(s, rdp->autodetect->netCharAverageRTT); /* averageRTT (4 bytes) */
			break;

		case 0x08C0:

			/* baseRTT, bandwidth, and averageRTT fields are present */
			if ((autodetectReqPdu->headerLength != 0x12) || (Stream_GetRemainingLength(s) < 12))
				return FALSE;

			Stream_Read_UINT32(s, rdp->autodetect->netCharBaseRTT);    /* baseRTT (4 bytes) */
			Stream_Read_UINT32(s, rdp->autodetect->netCharBandwidth);  /* bandwidth (4 bytes) */
			Stream_Read_UINT32(s, rdp->autodetect->netCharAverageRTT); /* averageRTT (4 bytes) */
			break;
	}

	WLog_VRB(AUTODETECT_TAG,
	         "received Network Characteristics Result PDU -> baseRTT=%" PRIu32
	         ", bandwidth=%" PRIu32 ", averageRTT=%" PRIu32 "",
	         rdp->autodetect->netCharBaseRTT, rdp->autodetect->netCharBandwidth,
	         rdp->autodetect->netCharAverageRTT);
	IFCALLRET(rdp->autodetect->NetworkCharacteristicsResult, success, rdp->context,
	          autodetectReqPdu->sequenceNumber);
	return success;
}