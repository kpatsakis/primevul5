lka_proc_config(struct processor_instance *pi)
{
	io_printf(pi->io, "config|smtpd-version|%s\n", SMTPD_VERSION);
	io_printf(pi->io, "config|smtp-session-timeout|%d\n", SMTPD_SESSION_TIMEOUT);
	if (pi->subsystems & FILTER_SUBSYSTEM_SMTP_IN)
		io_printf(pi->io, "config|subsystem|smtp-in\n");
	if (pi->subsystems & FILTER_SUBSYSTEM_SMTP_OUT)
		io_printf(pi->io, "config|subsystem|smtp-out\n");
	io_printf(pi->io, "config|admd|%s\n",
	    env->sc_admd != NULL ? env->sc_admd : env->sc_hostname);
	io_printf(pi->io, "config|ready\n");
}