static bool pmc_overflow(unsigned long val)
{
	if ((int)val < 0)
		return true;

	return false;
}