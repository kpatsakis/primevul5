static int link_to_linux_err(int link_status)
{
	if (link_status < 0) {
		/* status is already a Linux code */
		return link_status;
	}
	switch (link_status) {
	case MAC_SUCCESS:
	case MAC_REALIGNMENT:
		return 0;
	case MAC_IMPROPER_KEY_TYPE:
		return -EKEYREJECTED;
	case MAC_IMPROPER_SECURITY_LEVEL:
	case MAC_UNSUPPORTED_LEGACY:
	case MAC_DENIED:
		return -EACCES;
	case MAC_BEACON_LOST:
	case MAC_NO_ACK:
	case MAC_NO_BEACON:
		return -ENETUNREACH;
	case MAC_CHANNEL_ACCESS_FAILURE:
	case MAC_TX_ACTIVE:
	case MAC_SCAN_IN_PROGRESS:
		return -EBUSY;
	case MAC_DISABLE_TRX_FAILURE:
	case MAC_OUT_OF_CAP:
		return -EAGAIN;
	case MAC_FRAME_TOO_LONG:
		return -EMSGSIZE;
	case MAC_INVALID_GTS:
	case MAC_PAST_TIME:
		return -EBADSLT;
	case MAC_INVALID_HANDLE:
		return -EBADMSG;
	case MAC_INVALID_PARAMETER:
	case MAC_UNSUPPORTED_ATTRIBUTE:
	case MAC_ON_TIME_TOO_LONG:
	case MAC_INVALID_INDEX:
		return -EINVAL;
	case MAC_NO_DATA:
		return -ENODATA;
	case MAC_NO_SHORT_ADDRESS:
		return -EFAULT;
	case MAC_PAN_ID_CONFLICT:
		return -EADDRINUSE;
	case MAC_TRANSACTION_EXPIRED:
		return -ETIME;
	case MAC_TRANSACTION_OVERFLOW:
		return -ENOBUFS;
	case MAC_UNAVAILABLE_KEY:
		return -ENOKEY;
	case MAC_INVALID_ADDRESS:
		return -ENXIO;
	case MAC_TRACKING_OFF:
	case MAC_SUPERFRAME_OVERLAP:
		return -EREMOTEIO;
	case MAC_LIMIT_REACHED:
		return -EDQUOT;
	case MAC_READ_ONLY:
		return -EROFS;
	default:
		return -EPROTO;
	}
}