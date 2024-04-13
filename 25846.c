void ZipFile::init()
{
    std::unique_ptr<InputStream> toDelete;
    InputStream* in = inputStream;

    if (inputSource != nullptr)
    {
        in = inputSource->createInputStream();
        toDelete.reset (in);
    }

    if (in != nullptr)
    {
        int numEntries = 0;
        auto centralDirectoryPos = findCentralDirectoryFileHeader (*in, numEntries);

        if (centralDirectoryPos >= 0 && centralDirectoryPos < in->getTotalLength())
        {
            auto size = (size_t) (in->getTotalLength() - centralDirectoryPos);

            in->setPosition (centralDirectoryPos);
            MemoryBlock headerData;

            if (in->readIntoMemoryBlock (headerData, (ssize_t) size) == size)
            {
                size_t pos = 0;

                for (int i = 0; i < numEntries; ++i)
                {
                    if (pos + 46 > size)
                        break;

                    auto* buffer = static_cast<const char*> (headerData.getData()) + pos;
                    auto fileNameLen = readUnalignedLittleEndianShort (buffer + 28u);

                    if (pos + 46 + fileNameLen > size)
                        break;

                    entries.add (new ZipEntryHolder (buffer, fileNameLen));

                    pos += 46u + fileNameLen
                            + readUnalignedLittleEndianShort (buffer + 30u)
                            + readUnalignedLittleEndianShort (buffer + 32u);
                }
            }
        }
    }
}