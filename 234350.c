do_shell()
{
    screen_ok = FALSE;
    c_token++;

    if (user_shell) {
	if (system(user_shell) == -1)
	    os_error(NO_CARET, "system() failed");
    }
    (void) putc('\n', stderr);
}