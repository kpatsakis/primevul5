    void runTest() override
    {
        beginTest ("ZIP");

        StringArray entryNames { "first", "second", "third" };
        auto data = createZipMemoryBlock (entryNames);
        MemoryInputStream mi (data, false);
        ZipFile zip (mi);

        expectEquals (zip.getNumEntries(), entryNames.size());

        for (auto& entryName : entryNames)
        {
            auto* entry = zip.getEntry (entryName);
            std::unique_ptr<InputStream> input (zip.createStreamForEntry (*entry));
            expectEquals (input->readEntireStreamAsString(), entryName);
        }

        beginTest ("ZipSlip");
        runZipSlipTest();
    }