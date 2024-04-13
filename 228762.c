static void SFDConsumeUntil( FILE *sfd, const char** terminators ) {

    char* line = 0;
    while((line = getquotedeol( sfd ))) {
        const char** tp = terminators;
        for( ; tp && *tp; ++tp ) {
            if( !strnmatch( line, *tp, strlen( *tp ))) {
                free(line);
                return;
            }
        }
        free(line);
    }
}