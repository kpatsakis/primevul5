static int 	kssl_test_confound(unsigned char *p)
	{
	int 	len = 2;
	int 	xx = 0, yy = 0;

	if (*p++ != 0x62)  return 0;
	if (*p > 0x82)  return 0;
	switch(*p)  {
		case 0x82:  p++;          xx = (*p++ << 8);  xx += *p++;  break;
		case 0x81:  p++;          xx =  *p++;  break;
		case 0x80:  return 0;
		default:    xx = *p++;  break;
		}
	if (*p++ != 0x30)  return 0;
	if (*p > 0x82)  return 0;
	switch(*p)  {
		case 0x82:  p++; len+=2;  yy = (*p++ << 8);  yy += *p++;  break;
		case 0x81:  p++; len++;   yy =  *p++;  break;
		case 0x80:  return 0;
		default:    yy = *p++;  break;
		}

	return (xx - len == yy)? 1: 0;
	}