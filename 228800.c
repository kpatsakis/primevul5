void SFTimesFromFile(SplineFont *sf,FILE *file) {
    struct stat b;
    if ( fstat(fileno(file),&b)!=-1 ) {
	sf->modificationtime = GetST_MTime(b);
	sf->creationtime = GetST_MTime(b);
    }
}