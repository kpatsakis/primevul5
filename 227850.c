static char* rdp_get_bitmap_codec_guid_name(const GUID* guid)
{
	RPC_STATUS rpc_status;

	if (UuidEqual(guid, &CODEC_GUID_REMOTEFX, &rpc_status))
		return "CODEC_GUID_REMOTEFX";
	else if (UuidEqual(guid, &CODEC_GUID_NSCODEC, &rpc_status))
		return "CODEC_GUID_NSCODEC";
	else if (UuidEqual(guid, &CODEC_GUID_IGNORE, &rpc_status))
		return "CODEC_GUID_IGNORE";
	else if (UuidEqual(guid, &CODEC_GUID_IMAGE_REMOTEFX, &rpc_status))
		return "CODEC_GUID_IMAGE_REMOTEFX";

#if defined(WITH_JPEG)
	else if (UuidEqual(guid, &CODEC_GUID_JPEG, &rpc_status))
		return "CODEC_GUID_JPEG";

#endif
	return "CODEC_GUID_UNKNOWN";
}