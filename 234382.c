restore_prompt()
{
    if (interactive) {
#if defined(HAVE_LIBREADLINE) || defined(HAVE_LIBEDITLINE)
#  if !defined(MISSING_RL_FORCED_UPDATE_DISPLAY)
	rl_forced_update_display();
#  else
	rl_redisplay();
#  endif
#else
	fputs(PROMPT, stderr);
	fflush(stderr);
#endif
    }
}