R_API void r_io_bank_clear(RIOBank *bank) {
	r_return_if_fail (bank);
	while (!r_queue_is_empty (bank->todo)) {
		free (r_queue_dequeue (bank->todo));
	}
	bank->last_used = NULL;
	r_crbtree_clear (bank->submaps);
	r_list_purge (bank->maprefs);
}