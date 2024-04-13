static void rdp_read_capability_set_header(wStream* s, UINT16* length, UINT16* type)
{
	Stream_Read_UINT16(s, *type);   /* capabilitySetType */
	Stream_Read_UINT16(s, *length); /* lengthCapability */
}