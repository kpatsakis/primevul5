R_API void r_io_bank_del(RIO *io, const ut32 bankid) {
	r_id_storage_delete (io->banks, bankid);
	if (io->bank == bankid) {
		io->bank = r_io_bank_first (io);
	}
}