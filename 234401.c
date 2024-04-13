report_error(int ierr)
{
    int reported_error;

    /* FIXME:  This does not seem to report all reasonable errors correctly */
    if (ierr == -1 && errno != 0)
	reported_error = errno;
    else
	reported_error = WEXITSTATUS(ierr);

    fill_gpval_integer("GPVAL_SYSTEM_ERRNO", reported_error);
    if (reported_error == 127)
	fill_gpval_string("GPVAL_SYSTEM_ERRMSG", "command not found or shell failed");
    else
	fill_gpval_string("GPVAL_SYSTEM_ERRMSG", strerror(reported_error));

    return reported_error;
}