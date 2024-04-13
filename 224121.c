void signal_setup_done(int failed, struct ksignal *ksig, int stepping)
{
	if (failed)
		force_sigsegv(ksig->sig);
	else
		signal_delivered(ksig, stepping);
}