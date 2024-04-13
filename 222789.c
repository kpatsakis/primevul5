static inline bool is_error_page(struct page *page)
{
	return IS_ERR(page);
}