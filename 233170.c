aff_check_number(int spinval, int affval, char *name)
{
    if (spinval != 0 && spinval != affval)
	smsg((char_u *)_("%s value differs from what is used in another .aff file"), name);
}