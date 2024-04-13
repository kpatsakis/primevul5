ex_ni(exarg_T *eap)
{
    if (!eap->skip)
	eap->errmsg =
		_("E319: Sorry, the command is not available in this version");
}