int LibRaw::setMakeFromIndex(unsigned makei)
{
	if (makei <= LIBRAW_CAMERAMAKER_Unknown || makei >= LIBRAW_CAMERAMAKER_TheLastOne) return 0;

	for (int i = 0; i < int(sizeof CorpTable / sizeof *CorpTable); i++)
		if ((unsigned)CorpTable[i].CorpId == makei)
		{
			strcpy(normalized_make, CorpTable[i].CorpName);
			maker_index = makei;
			return 1;
		}
	return 0;
}