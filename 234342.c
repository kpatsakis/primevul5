do_shell()
{
    static char exec[100] = EXEC;

    screen_ok = FALSE;
    c_token++;

    if (user_shell) {
	if (system(safe_strncpy(&exec[sizeof(EXEC) - 1], user_shell,
				sizeof(exec) - sizeof(EXEC) - 1)))
	    os_error(NO_CARET, "system() failed");
    }
    (void) putc('\n', stderr);
}