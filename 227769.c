static BOOL rdp_write_brush_capability_set(wStream* s, const rdpSettings* settings)
{
	size_t header;

	if (!Stream_EnsureRemainingCapacity(s, 32))
		return FALSE;

	header = rdp_capability_set_start(s);
	if (header > UINT16_MAX)
		return FALSE;
	Stream_Write_UINT32(s, settings->BrushSupportLevel); /* brushSupportLevel (4 bytes) */
	rdp_capability_set_finish(s, (UINT16)header, CAPSET_TYPE_BRUSH);
	return TRUE;
}