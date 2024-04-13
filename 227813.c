static BOOL rdp_write_desktop_composition_capability_set(wStream* s, const rdpSettings* settings)
{
	size_t header;
	UINT16 compDeskSupportLevel;

	if (!Stream_EnsureRemainingCapacity(s, 32))
		return FALSE;

	header = rdp_capability_set_start(s);
	compDeskSupportLevel =
	    (settings->AllowDesktopComposition) ? COMPDESK_SUPPORTED : COMPDESK_NOT_SUPPORTED;
	Stream_Write_UINT16(s, compDeskSupportLevel); /* compDeskSupportLevel (2 bytes) */
	rdp_capability_set_finish(s, header, CAPSET_TYPE_COMP_DESK);
	return TRUE;
}