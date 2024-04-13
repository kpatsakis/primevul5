estimate_plaintext(char *enhancedtext)
{
    if (enhancedtext == NULL)
	return NULL;
    estimate_strlen(enhancedtext, NULL);
    return ENHest_plaintext;
}