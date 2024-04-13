static int64 findCentralDirectoryFileHeader (InputStream& input, int& numEntries)
{
    BufferedInputStream in (input, 8192);

    in.setPosition (in.getTotalLength());
    auto pos = in.getPosition();
    auto lowestPos = jmax ((int64) 0, pos - 1048576);
    char buffer[32] = {};

    while (pos > lowestPos)
    {
        in.setPosition (pos - 22);
        pos = in.getPosition();
        memcpy (buffer + 22, buffer, 4);

        if (in.read (buffer, 22) != 22)
            return 0;

        for (int i = 0; i < 22; ++i)
        {
            if (readUnalignedLittleEndianInt (buffer + i) == 0x06054b50)
            {
                in.setPosition (pos + i);
                in.read (buffer, 22);
                numEntries = readUnalignedLittleEndianShort (buffer + 10);
                auto offset = (int64) readUnalignedLittleEndianInt (buffer + 16);

                if (offset >= 4)
                {
                    in.setPosition (offset);

                    // This is a workaround for some zip files which seem to contain the
                    // wrong offset for the central directory - instead of including the
                    // header, they point to the byte immediately after it.
                    if (in.readInt() != 0x02014b50)
                    {
                        in.setPosition (offset - 4);

                        if (in.readInt() == 0x02014b50)
                            offset -= 4;
                    }
                }

                return offset;
            }
        }
    }

    return 0;
}