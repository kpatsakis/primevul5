static bool __reg64_bound_s32(s64 a)
{
	if (a > S32_MIN && a < S32_MAX)
		return true;
	return false;
}