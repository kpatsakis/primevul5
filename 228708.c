static int nlgetc(FILE *sfd) {
    int ch, ch2;

    ch=getc(sfd);
    if ( ch!='\\' )
return( ch );
    ch2 = getc(sfd);
    if ( ch2=='\n' )
return( nlgetc(sfd));
    ungetc(ch2,sfd);
return( ch );
}