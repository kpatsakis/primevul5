kssl_ctx_setstring(KSSL_CTX *kssl_ctx, int which, char *text)
        {
	char	**string;

	if (!kssl_ctx)  return KSSL_CTX_ERR;

	switch (which)
                {
        case KSSL_SERVICE:	string = &kssl_ctx->service_name;	break;
        case KSSL_SERVER:	string = &kssl_ctx->service_host;	break;
        case KSSL_CLIENT:	string = &kssl_ctx->client_princ;	break;
        case KSSL_KEYTAB:	string = &kssl_ctx->keytab_file;	break;
        default:		return KSSL_CTX_ERR;			break;
		}
	if (*string)  kssl_free(*string);

	if (!text)
                {
		*string = '\0';
		return KSSL_CTX_OK;
		}

	if ((*string = kssl_calloc(1, strlen(text) + 1)) == NULL)
		return KSSL_CTX_ERR;
	else
		strcpy(*string, text);

	return KSSL_CTX_OK;
        }