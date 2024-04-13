void ZipFile::sortEntriesByFilename()
{
    std::sort (entries.begin(), entries.end(),
               [] (const ZipEntryHolder* e1, const ZipEntryHolder* e2) { return e1->entry.filename < e2->entry.filename; });
}