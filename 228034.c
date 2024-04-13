f_getcharstr(typval_T *argvars, typval_T *rettv)
{
    getchar_common(argvars, rettv);

    if (rettv->v_type == VAR_NUMBER)
    {
	char_u		temp[7];   // mbyte-char: 6, NUL: 1
	varnumber_T	n = rettv->vval.v_number;
	int		i = 0;

	if (n != 0)
	{
	    if (has_mbyte)
		i += (*mb_char2bytes)(n, temp + i);
	    else
		temp[i++] = n;
	}
	temp[i++] = NUL;
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = vim_strsave(temp);
    }
}