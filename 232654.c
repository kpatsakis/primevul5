static bool is_uimm32(u64 value)
{
	return value == (u64)(u32)value;
}