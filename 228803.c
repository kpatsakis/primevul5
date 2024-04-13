FILE* MakeTemporaryFile(void) {
    FILE *ret = NULL;

#ifndef __MINGW32__
    gchar *loc;
    int fd = g_file_open_tmp("fontforge-XXXXXX", &loc, NULL);

    if (fd != -1) {
        ret = fdopen(fd, "w+");
        g_unlink(loc);
        g_free(loc);
    }
#else
    HANDLE hFile = INVALID_HANDLE_VALUE;
    for (int retries = 0; hFile == INVALID_HANDLE_VALUE && retries < 10; retries++) {
        wchar_t *temp = _wtempnam(NULL, L"FontForge");
        hFile = CreateFileW(temp, GENERIC_READ|GENERIC_WRITE, 0, NULL,
            CREATE_NEW, FILE_ATTRIBUTE_TEMPORARY|FILE_FLAG_DELETE_ON_CLOSE, NULL);
        free(temp);
    }
    ret = _fdopen(_open_osfhandle((intptr_t)hFile, 0), "wb+");
    if (ret == NULL && hFile != INVALID_HANDLE_VALUE) {
        CloseHandle(hFile);
    }
#endif
    return ret;
}