enum siginfo_layout siginfo_layout(unsigned sig, int si_code)
{
	enum siginfo_layout layout = SIL_KILL;
	if ((si_code > SI_USER) && (si_code < SI_KERNEL)) {
		if ((sig < ARRAY_SIZE(sig_sicodes)) &&
		    (si_code <= sig_sicodes[sig].limit)) {
			layout = sig_sicodes[sig].layout;
			/* Handle the exceptions */
			if ((sig == SIGBUS) &&
			    (si_code >= BUS_MCEERR_AR) && (si_code <= BUS_MCEERR_AO))
				layout = SIL_FAULT_MCEERR;
			else if ((sig == SIGSEGV) && (si_code == SEGV_BNDERR))
				layout = SIL_FAULT_BNDERR;
#ifdef SEGV_PKUERR
			else if ((sig == SIGSEGV) && (si_code == SEGV_PKUERR))
				layout = SIL_FAULT_PKUERR;
#endif
		}
		else if (si_code <= NSIGPOLL)
			layout = SIL_POLL;
	} else {
		if (si_code == SI_TIMER)
			layout = SIL_TIMER;
		else if (si_code == SI_SIGIO)
			layout = SIL_POLL;
		else if (si_code < 0)
			layout = SIL_RT;
	}
	return layout;
}