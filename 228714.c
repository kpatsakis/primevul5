static void SFDDumpEncoding(FILE *sfd,Encoding *encname,const char *keyword) {
    fprintf(sfd, "%s: %s\n", keyword, encname->enc_name );
}