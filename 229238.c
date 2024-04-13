check_login_time(const char *ruser, time_t timestamp)
{
	struct utmp utbuf, *ut;
	time_t oldest_login = 0;

	setutent();
	while(
#ifdef HAVE_GETUTENT_R
	      !getutent_r(&utbuf, &ut)
#else
	      (ut = getutent()) != NULL
#endif
	      ) {
		if (ut->ut_type != USER_PROCESS) {
			continue;
		}
		if (strncmp(ruser, ut->ut_user, sizeof(ut->ut_user) != 0)) {
			continue;
		}
		if (oldest_login == 0 || oldest_login > ut->ut_tv.tv_sec) {
			oldest_login = ut->ut_tv.tv_sec;
		}
	}
	endutent();
	if(oldest_login == 0 || timestamp < oldest_login) {
		return PAM_AUTH_ERR;
	}
	return PAM_SUCCESS;
}