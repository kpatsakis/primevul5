    static MemoryBlock createZipMemoryBlock (const StringArray& entryNames)
    {
        ZipFile::Builder builder;
        HashMap<String, MemoryBlock> blocks;

        for (auto& entryName : entryNames)
        {
            auto& block = blocks.getReference (entryName);
            MemoryOutputStream mo (block, false);
            mo << entryName;
            mo.flush();
            builder.addEntry (new MemoryInputStream (block, false), 9, entryName, Time::getCurrentTime());
        }

        MemoryBlock data;
        MemoryOutputStream mo (data, false);
        builder.writeToStream (mo, nullptr);

        return data;
    }