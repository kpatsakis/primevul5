R_API void r_io_bank_free(RIOBank *bank) {
	if (bank) {
		r_queue_free (bank->todo);
		r_list_free (bank->maprefs);
		r_crbtree_free (bank->submaps);
		free (bank->name);
		free (bank);
	}
}