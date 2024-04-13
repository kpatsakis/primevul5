static struct tm *k_gmtime(ASN1_GENERALIZEDTIME *gtime, struct tm *k_tm)
	{
	char 		c, *p;

	if (!k_tm)  return NULL;
	if (gtime == NULL  ||  gtime->length < 14)  return NULL;
	if (gtime->data == NULL)  return NULL;

	p = (char *)&gtime->data[14];

	c = *p;	 *p = '\0';  p -= 2;  k_tm->tm_sec  = atoi(p);      *(p+2) = c;
	c = *p;	 *p = '\0';  p -= 2;  k_tm->tm_min  = atoi(p);      *(p+2) = c;
	c = *p;	 *p = '\0';  p -= 2;  k_tm->tm_hour = atoi(p);      *(p+2) = c;
	c = *p;	 *p = '\0';  p -= 2;  k_tm->tm_mday = atoi(p);      *(p+2) = c;
	c = *p;	 *p = '\0';  p -= 2;  k_tm->tm_mon  = atoi(p)-1;    *(p+2) = c;
	c = *p;	 *p = '\0';  p -= 4;  k_tm->tm_year = atoi(p)-1900; *(p+4) = c;

	return k_tm;
	}