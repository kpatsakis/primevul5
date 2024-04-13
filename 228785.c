static uint32 gettag(FILE *sfd) {
    int ch, quoted;
    uint32 tag;

    while ( (ch=nlgetc(sfd))==' ' );
    if ( (quoted = (ch=='\'')) ) ch = nlgetc(sfd);
    tag = (ch<<24)|(nlgetc(sfd)<<16);
    tag |= nlgetc(sfd)<<8;
    tag |= nlgetc(sfd);
    if ( quoted ) (void) nlgetc(sfd);
return( tag );
}