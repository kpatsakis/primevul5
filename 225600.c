static void iwl_dump_ini_list_free(struct list_head *list)
{
	while (!list_empty(list)) {
		struct iwl_fw_ini_dump_entry *entry =
			list_entry(list->next, typeof(*entry), list);

		list_del(&entry->list);
		kfree(entry);
	}
}