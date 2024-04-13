bool ZipFile::Builder::writeToStream (OutputStream& target, double* const progress) const
{
    auto fileStart = target.getPosition();

    for (int i = 0; i < items.size(); ++i)
    {
        if (progress != nullptr)
            *progress = (i + 0.5) / items.size();

        if (! items.getUnchecked (i)->writeData (target, fileStart))
            return false;
    }

    auto directoryStart = target.getPosition();

    for (auto* item : items)
        if (! item->writeDirectoryEntry (target))
            return false;

    auto directoryEnd = target.getPosition();

    target.writeInt (0x06054b50);
    target.writeShort (0);
    target.writeShort (0);
    target.writeShort ((short) items.size());
    target.writeShort ((short) items.size());
    target.writeInt ((int) (directoryEnd - directoryStart));
    target.writeInt ((int) (directoryStart - fileStart));
    target.writeShort (0);

    if (progress != nullptr)
        *progress = 1.0;

    return true;
}