static void dump_one_vdso_page(struct page *pg, struct page *upg)
{
	printk("kpg: %p (c:%d,f:%08lx)", __va(page_to_pfn(pg) << PAGE_SHIFT),
	       page_count(pg),
	       pg->flags);
	if (upg && !IS_ERR(upg) /* && pg != upg*/) {
		printk(" upg: %p (c:%d,f:%08lx)", __va(page_to_pfn(upg)
						       << PAGE_SHIFT),
		       page_count(upg),
		       upg->flags);
	}
	printk("\n");
}