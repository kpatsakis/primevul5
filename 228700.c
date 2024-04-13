static void utf7_encode(FILE *sfd,long ch) {

    putc(base64[(ch>>18)&0x3f],sfd);
    putc(base64[(ch>>12)&0x3f],sfd);
    putc(base64[(ch>>6)&0x3f],sfd);
    putc(base64[ch&0x3f],sfd);
}