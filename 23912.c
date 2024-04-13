print_krb5_authdata(char *label, krb5_authdata **adata)
        {
	if (adata == NULL)
                {
		printf("%s, authdata==0\n", label);
		return;
		}
	printf("%s [%p]\n", label, (void *)adata);
#if 0
	{
        int 	i;
	printf("%s[at%d:%d] ", label, adata->ad_type, adata->length);
	for (i=0; i < adata->length; i++)
                {
                printf((isprint(adata->contents[i]))? "%c ": "%02x",
                        adata->contents[i]);
		}
	printf("\n");
	}
#endif
	}