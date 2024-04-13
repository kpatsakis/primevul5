void mm_put_huge_zero_page(struct mm_struct *mm)
{
	if (test_bit(MMF_HUGE_ZERO_PAGE, &mm->flags))
		put_huge_zero_page();
}