int getname(FILE *sfd, char *tokbuf) {
    int ch;

    while ( isspace(ch = nlgetc(sfd)));
    ungetc(ch,sfd);
return( getprotectedname(sfd,tokbuf));
}