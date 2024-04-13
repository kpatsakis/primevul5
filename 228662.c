static void SFDDumpLangName(FILE *sfd, struct ttflangname *ln) {
    int i, end;
    fprintf( sfd, "LangName: %d", ln->lang );
    for ( end = ttf_namemax; end>0 && ln->names[end-1]==NULL; --end );
    for ( i=0; i<end; ++i ) {
        putc(' ',sfd);
        SFDDumpUTF7Str(sfd,ln->names[i]);
    }
    putc('\n',sfd);
}