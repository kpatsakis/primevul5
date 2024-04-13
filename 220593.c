ex_mode(exarg_T *eap)
{
    if (*eap->arg == NUL)
	shell_resized();
    else
	emsg(_(e_screenmode));
}