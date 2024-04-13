help_command()
{
    HWND parent;

    c_token++;
    parent = GetDesktopWindow();

    /* open help file if necessary */
    help_window = HtmlHelp(parent, winhelpname, HH_GET_WIN_HANDLE, (DWORD_PTR)NULL);
    if (help_window == NULL) {
	help_window = HtmlHelp(parent, winhelpname, HH_DISPLAY_TOPIC, (DWORD_PTR)NULL);
	if (help_window == NULL) {
	    fprintf(stderr, "Error: Could not open help file \"" TCHARFMT "\"\n", winhelpname);
	    return;
	}
    }
    if (END_OF_COMMAND) {
	/* show table of contents */
	HtmlHelp(parent, winhelpname, HH_DISPLAY_TOC, (DWORD_PTR)NULL);
    } else {
	/* lookup topic in index */
	HH_AKLINK link;
	char buf[128];
#ifdef UNICODE
	WCHAR wbuf[128];
#endif
	int start = c_token;
	while (!(END_OF_COMMAND))
	    c_token++;
	capture(buf, start, c_token - 1, sizeof(buf));
	link.cbStruct =     sizeof(HH_AKLINK) ;
	link.fReserved =    FALSE;
#ifdef UNICODE
	MultiByteToWideChar(WinGetCodepage(encoding), 0, buf, sizeof(buf), wbuf, sizeof(wbuf) / sizeof(WCHAR));
	link.pszKeywords =  wbuf;
#else
	link.pszKeywords =  buf;
#endif
	link.pszUrl =       NULL;
	link.pszMsgText =   NULL;
	link.pszMsgTitle =  NULL;
	link.pszWindow =    NULL;
	link.fIndexOnFail = TRUE;
	HtmlHelp(parent, winhelpname, HH_KEYWORD_LOOKUP, (DWORD_PTR)&link);
    }
}