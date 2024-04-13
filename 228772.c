int SFDDoesAnyBackupExist(char* filename)
{
    char path[PATH_MAX];
    int idx = 1;

    snprintf( path, PATH_MAX, "%s-%02d", filename, idx );
    return GFileExists(path);
}