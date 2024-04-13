msg_add_fileformat(int eol_type)
{
#ifndef USE_CRNL
    if (eol_type == EOL_DOS)
    {
	STRCAT(IObuff, shortmess(SHM_TEXT) ? _("[dos]") : _("[dos format]"));
	return TRUE;
    }
#endif
#ifndef USE_CR
    if (eol_type == EOL_MAC)
    {
	STRCAT(IObuff, shortmess(SHM_TEXT) ? _("[mac]") : _("[mac format]"));
	return TRUE;
    }
#endif
#if defined(USE_CRNL) || defined(USE_CR)
    if (eol_type == EOL_UNIX)
    {
	STRCAT(IObuff, shortmess(SHM_TEXT) ? _("[unix]") : _("[unix format]"));
	return TRUE;
    }
#endif
    return FALSE;
}