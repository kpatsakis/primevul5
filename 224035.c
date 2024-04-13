bool is_sier_available(void)
{
	if (!ppmu)
		return false;

	if (ppmu->flags & PPMU_HAS_SIER)
		return true;

	return false;
}