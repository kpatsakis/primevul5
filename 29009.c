static int CMSError(int ecode, const char *msg)
{
    error(-1,const_cast<char *>(msg));
    return 1;
}
