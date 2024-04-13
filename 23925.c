print_krb5_data(char *label, krb5_data *kdata)
        {
	int i;

	printf("%s[%d] ", label, kdata->length);
	for (i=0; i < (int)kdata->length; i++)
                {
		if (0 &&  isprint((int) kdata->data[i]))
                        printf(	"%c ",  kdata->data[i]);
		else
                        printf(	"%02x ", (unsigned char) kdata->data[i]);
		}
	printf("\n");
        }