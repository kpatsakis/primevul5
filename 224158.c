static bool known_siginfo_layout(unsigned sig, int si_code)
{
	if (si_code == SI_KERNEL)
		return true;
	else if ((si_code > SI_USER)) {
		if (sig_specific_sicodes(sig)) {
			if (si_code <= sig_sicodes[sig].limit)
				return true;
		}
		else if (si_code <= NSIGPOLL)
			return true;
	}
	else if (si_code >= SI_DETHREAD)
		return true;
	else if (si_code == SI_ASYNCNL)
		return true;
	return false;
}