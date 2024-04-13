do_shell()
{
    screen_ok = FALSE;
    c_token++;

    if (user_shell) {
#  if defined(_WIN32)
	if (WinExec(user_shell, SW_SHOWNORMAL) <= 32)
#  elif defined(__DJGPP__)
	if (system(user_shell) == -1)
#  else
	if (spawnl(P_WAIT, user_shell, NULL) == -1)
#  endif /* !(_WIN32 || __DJGPP__) */
	    os_error(NO_CARET, "unable to spawn shell");
    }
}