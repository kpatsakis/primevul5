aff_check_string(char_u *spinval, char_u *affval, char *name)
{
    if (spinval != NULL && STRCMP(spinval, affval) != 0)
	smsg((char_u *)_("%s value differs from what is used in another .aff file"), name);
}