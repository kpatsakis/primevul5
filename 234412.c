do_shell()
{
    screen_ok = FALSE;
    c_token++;

    if ((vaxc$errno = lib$spawn()) != SS$_NORMAL) {
	os_error(NO_CARET, "spawn error");
    }
}