static inline void add_mm_rss(struct mm_struct *mm, int file_rss, int anon_rss)
{
	if (file_rss)
		add_mm_counter(mm, file_rss, file_rss);
	if (anon_rss)
		add_mm_counter(mm, anon_rss, anon_rss);
}