print_krb5_keyblock(char *label, krb5_keyblock *keyblk)
        {
	int i;

	if (keyblk == NULL)
                {
		printf("%s, keyblk==0\n", label);
		return;
		}
#ifdef KRB5_HEIMDAL
	printf("%s\n\t[et%d:%d]: ", label, keyblk->keytype,
					   keyblk->keyvalue->length);
	for (i=0; i < (int)keyblk->keyvalue->length; i++)
                {
		printf("%02x",(unsigned char *)(keyblk->keyvalue->contents)[i]);
		}
	printf("\n");
#else
	printf("%s\n\t[et%d:%d]: ", label, keyblk->enctype, keyblk->length);
	for (i=0; i < (int)keyblk->length; i++)
                {
		printf("%02x",keyblk->contents[i]);
		}
	printf("\n");
#endif
        }