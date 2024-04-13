termcomp(const generic *arga, const generic *argb)
{
    const int *a = arga;
    const int *b = argb;

    return( strcasecmp( term_tbl[*a].name, term_tbl[*b].name ) );
}