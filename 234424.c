do_system(const char *cmd)
{
    int ierr;

/* (am, 19980929)
 * OS/2 related note: cmd.exe returns 255 if called w/o argument.
 * i.e. calling a shell by "!" will always end with an error message.
 * A workaround has to include checking for EMX,OS/2, two environment
 *  variables,...
 */
    if (!cmd)
	return;
    restrict_popen();
#if defined(_WIN32) && !defined(WGP_CONSOLE)
    /* Open a console so we can see the command's output */
    WinOpenConsole();
#endif
#if defined(_WIN32) && !defined(HAVE_BROKEN_WSYSTEM)
    {
	LPWSTR wcmd = UnicodeText(cmd, encoding);
	ierr = _wsystem(wcmd);
	free(wcmd);
    }
#else
    ierr = system(cmd);
#endif
    report_error(ierr);
}