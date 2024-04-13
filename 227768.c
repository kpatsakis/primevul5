static void rdp_write_capability_set_header(wStream* s, UINT16 length, UINT16 type)
{
	Stream_Write_UINT16(s, type);   /* capabilitySetType */
	Stream_Write_UINT16(s, length); /* lengthCapability */
}