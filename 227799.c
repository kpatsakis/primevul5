static const char* get_capability_name(UINT16 type)
{
	if (type > CAPSET_TYPE_FRAME_ACKNOWLEDGE)
		return "<unknown>";

	return CAPSET_TYPE_STRINGS[type];
}