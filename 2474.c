R_API ut32 r_io_bank_first(RIO *io) {
	ut32 bankid = -1;
	r_id_storage_get_lowest (io->banks, &bankid);
	return bankid;
}