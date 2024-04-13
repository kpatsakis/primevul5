set_forced_fenc(exarg_T *eap)
{
    if (eap->force_enc != 0)
    {
	char_u *fenc = enc_canonize(eap->cmd + eap->force_enc);

	if (fenc != NULL)
	    set_string_option_direct((char_u *)"fenc", -1,
				 fenc, OPT_FREE|OPT_LOCAL, 0);
	vim_free(fenc);
    }
}