R_API bool r_io_bank_use(RIO *io, ut32 bankid) {
	RIOBank *bank = r_io_bank_get (io, bankid);
	if (bank) {
		io->bank = bankid;
		return true;
	}
	return false;
}