static double SFDStartsCorrectly(FILE *sfd,char *tok) {
    real dval;
    int ch;

    if ( getname(sfd,tok)!=1 )
return( -1 );
    if ( strcmp(tok,"SplineFontDB:")!=0 )
return( -1 );
    if ( getreal(sfd,&dval)!=1 )
return( -1 );
    /* We don't yet generate version 4 of sfd. It will contain backslash */
    /*  newline in the middle of very long lines. I've put in code to parse */
    /*  this sequence, but I don't yet generate it. I want the parser to */
    /*  perculate through to users before I introduce the new format so there */
    /*  will be fewer complaints when it happens */
    // MIQ: getreal() can give some funky rounding errors it seems
    if ( dval!=0 && dval!=1 && dval!=2.0 && dval!=3.0
         && !(dval > 3.09 && dval <= 3.21)
         && dval!=4.0 )
    {
        LogError("Bad SFD Version number %.1f", dval );
return( -1 );
    }
    ch = nlgetc(sfd); ungetc(ch,sfd);
    if ( ch!='\r' && ch!='\n' )
return( -1 );

return( dval );
}