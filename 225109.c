int RemoveSectionType(int SectionType)
{
    int a;
    int retval = FALSE;
    for (a=0;a<SectionsRead-1;a++){
        if (Sections[a].Type == SectionType){
            // Free up this section
            free (Sections[a].Data);
            // Move succeding sections back by one to close space in array.
            memmove(Sections+a, Sections+a+1, sizeof(Section_t) * (SectionsRead-a));
            SectionsRead -= 1;
            a -= 1;
            retval = TRUE;
        }
    }
    return retval;
}