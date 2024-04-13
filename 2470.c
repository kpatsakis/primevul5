R_API bool r_io_bank_add(RIO *io, RIOBank *bank) {
	r_return_val_if_fail (io && io->banks && bank, false);
	return r_id_storage_add (io->banks, bank, &bank->id);
}