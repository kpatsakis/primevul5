Header headerCopyLoad(const void * uh)
{
    /* Discards const but that's ok as we'll take a copy */
    return headerImport((void *)uh, 0, HEADERIMPORT_COPY);
}