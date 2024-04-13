static BOOL rdp_write_sound_capability_set(wStream* s, const rdpSettings* settings)
{
	size_t header;
	UINT16 soundFlags;

	if (!Stream_EnsureRemainingCapacity(s, 32))
		return FALSE;

	header = rdp_capability_set_start(s);
	if (header > UINT16_MAX)
		return FALSE;
	soundFlags = (settings->SoundBeepsEnabled) ? SOUND_BEEPS_FLAG : 0;
	Stream_Write_UINT16(s, soundFlags); /* soundFlags (2 bytes) */
	Stream_Write_UINT16(s, 0);          /* pad2OctetsA (2 bytes) */
	rdp_capability_set_finish(s, (UINT16)header, CAPSET_TYPE_SOUND);
	return TRUE;
}