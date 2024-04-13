    void runZipSlipTest()
    {
        const std::map<String, bool> testCases = { { "a",                    true  },
#if JUCE_WINDOWS
                                                   { "C:/b",                 false },
#else
                                                   { "/b",                   false },
#endif
                                                   { "c/d",                  true  },
                                                   { "../e/f",               false },
                                                   { "../../g/h",            false },
                                                   { "i/../j",               true  },
                                                   { "k/l/../",              true  },
                                                   { "m/n/../../",           false },
                                                   { "o/p/../../../",        false } };

        StringArray entryNames;

        for (const auto& testCase : testCases)
            entryNames.add (testCase.first);

        TemporaryFile tmpDir;
        tmpDir.getFile().createDirectory();
        auto data = createZipMemoryBlock (entryNames);
        MemoryInputStream mi (data, false);
        ZipFile zip (mi);

        for (int i = 0; i < zip.getNumEntries(); ++i)
        {
            const auto result = zip.uncompressEntry (i, tmpDir.getFile());
            const auto caseIt = testCases.find (zip.getEntry (i)->filename);

            if (caseIt != testCases.end())
            {
                expect (result.wasOk() == caseIt->second,
                        zip.getEntry (i)->filename + " was unexpectedly " + (result.wasOk() ? "OK" : "not OK"));
            }
            else
            {
                expect (false);
            }
        }
    }