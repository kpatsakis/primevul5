stylefont(const char *fontname, TBOOLEAN isbold, TBOOLEAN isitalic)
{
    int div;
    char *markup = gp_alloc( strlen(fontname) + 16, "font markup");
    strcpy(markup, fontname);
    /* base font name can be followed by ,<size> or :Variant */
    if ((div = strcspn(markup,",:")))
	markup[div] = '\0';
    if (isbold)
	strcat(markup, ":Bold");
    if (isitalic)
	strcat(markup, ":Italic");

    FPRINTF((stderr, "MARKUP FONT: %s -> %s\n", fontname, markup));
    return markup;
}