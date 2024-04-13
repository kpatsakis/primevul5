print_krb5_princ(char *label, krb5_principal_data *princ)
        {
	int i, ui, uj;

	printf("%s principal Realm: ", label);
	if (princ == NULL)  return;
	for (ui=0; ui < (int)princ->realm.length; ui++)  putchar(princ->realm.data[ui]);
	printf(" (nametype %d) has %d strings:\n", princ->type,princ->length);
	for (i=0; i < (int)princ->length; i++)
                {
		printf("\t%d [%d]: ", i, princ->data[i].length);
		for (uj=0; uj < (int)princ->data[i].length; uj++)  {
			putchar(princ->data[i].data[uj]);
			}
		printf("\n");
		}
	return;
        }