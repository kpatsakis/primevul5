R_API RIOBank *r_io_bank_new(const char *name) {
	r_return_val_if_fail (name, NULL);
	RIOBank *bank = R_NEW0 (RIOBank);
	if (!bank) {
		return NULL;
	}
	bank->name = strdup (name);
	bank->submaps = r_crbtree_new (free);
	if (!bank->submaps) {
		free (bank);
		return NULL;
	}
	bank->maprefs = r_list_newf (free);
	if (!bank->maprefs) {
		r_crbtree_free (bank->submaps);
		free (bank);
		return NULL;
	}
	bank->todo = r_queue_new (8);
	if (!bank->todo) {
		r_list_free (bank->maprefs);
		r_crbtree_free (bank->submaps);
		free (bank);
		return NULL;
	}
	return bank;
}