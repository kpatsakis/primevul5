static size_t  *populate_cksumlens(void)
	{
	int 		i, j, n;
	static size_t 	*cklens = NULL;

#ifdef KRB5_MIT_OLD11
	n = krb5_max_cksum;
#else
	n = 0x0010;
#endif	/* KRB5_MIT_OLD11 */
 
#ifdef KRB5CHECKAUTH
	if (!cklens && !(cklens = (size_t *) calloc(sizeof(int),n+1)))  return NULL;

	for (i=0; i < n; i++)  {
		if (!valid_cksumtype(i))  continue;	/*  array has holes  */
		for (j=0; j < n; j++)  {
			if (cklens[j] == 0)  {
				cklens[j] = krb5_checksum_size(NULL,i);
				break;		/*  krb5 elem was new: add   */
				}
			if (cklens[j] == krb5_checksum_size(NULL,i))  {
				break;		/*  ignore duplicate elements */
				}
			}
		}
#endif	/* KRB5CHECKAUTH */

	return cklens;
	}