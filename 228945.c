static char priv_level_to_char(unsigned char priv_level)
{
	char ret = 'X';

	switch (priv_level)
	{
	case IPMI_SESSION_PRIV_CALLBACK:
		ret = 'c';
		break;
	case IPMI_SESSION_PRIV_USER:
		ret = 'u';
		break;
	case IPMI_SESSION_PRIV_OPERATOR:
		ret = 'o';
		break;
	case IPMI_SESSION_PRIV_ADMIN:
		ret = 'a';
		break;
	case IPMI_SESSION_PRIV_OEM:
		ret = 'O';
		break;
	}

 	return ret;
}