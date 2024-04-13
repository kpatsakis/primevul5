void set_dumpable(struct mm_struct *mm, int value)
{
	if (WARN_ON((unsigned)value > SUID_DUMP_ROOT))
		return;

	set_mask_bits(&mm->flags, MMF_DUMPABLE_MASK, value);
}