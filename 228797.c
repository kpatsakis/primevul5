static int getusint(FILE *sfd, uint16 *val) {
    int val2;
    int ret = getint(sfd,&val2);
    *val = val2;
return( ret );
}