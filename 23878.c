kssl_ctx_setprinc(KSSL_CTX *kssl_ctx, int which,
        krb5_data *realm, krb5_data *entity, int nentities)
        {
	char	**princ;
	int 	length;
	int i;

	if (kssl_ctx == NULL  ||  entity == NULL)  return KSSL_CTX_ERR;

	switch (which)
                {
        case KSSL_CLIENT:	princ = &kssl_ctx->client_princ;	break;
        case KSSL_SERVER:	princ = &kssl_ctx->service_host;	break;
        default:		return KSSL_CTX_ERR;			break;
		}
	if (*princ)  kssl_free(*princ);

	/* Add up all the entity->lengths */
	length = 0;
	for (i=0; i < nentities; i++)
		{
		length += entity[i].length;
		}
	/* Add in space for the '/' character(s) (if any) */
	length += nentities-1;
	/* Space for the ('@'+realm+NULL | NULL) */
	length += ((realm)? realm->length + 2: 1);

	if ((*princ = kssl_calloc(1, length)) == NULL)
		return KSSL_CTX_ERR;
	else
		{
		for (i = 0; i < nentities; i++)
			{
			strncat(*princ, entity[i].data, entity[i].length);
			if (i < nentities-1)
				{
				strcat (*princ, "/");
				}
			}
		if (realm)
                        {
			strcat (*princ, "@");
			(void) strncat(*princ, realm->data, realm->length);
			}
		}

	return KSSL_CTX_OK;
        }