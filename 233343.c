const char *LibRaw::cameramakeridx2maker(unsigned maker)
{
    for (int i = 0; i < int(sizeof CorpTable / sizeof *CorpTable); i++)
        if((unsigned)CorpTable[i].CorpId == maker)
            return CorpTable[i].CorpName;
    return 0;
}