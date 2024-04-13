UnicodeStringTest::TestSearching()
{
    UnicodeString test1("test test ttest tetest testesteststt");
    UnicodeString test2("test");
    UChar testChar = 0x74;
    
    UChar32 testChar32 = 0x20402;
    UChar testData[]={
        //   0       1       2       3       4       5       6       7
        0xd841, 0xdc02, 0x0071, 0xdc02, 0xd841, 0x0071, 0xd841, 0xdc02,

        //   8       9      10      11      12      13      14      15
        0x0071, 0x0072, 0xd841, 0xdc02, 0x0071, 0xd841, 0xdc02, 0x0071,

        //  16      17      18      19
        0xdc02, 0xd841, 0x0073, 0x0000
    };
    UnicodeString test3(testData);
    UnicodeString test4(testChar32);

    uint16_t occurrences = 0;
    int32_t startPos = 0;
    for ( ;
          startPos != -1 && startPos < test1.length();
          (startPos = test1.indexOf(test2, startPos)) != -1 ? (++occurrences, startPos += 4) : 0)
        ;
    if (occurrences != 6)
        errln(UnicodeString("indexOf failed: expected to find 6 occurrences, found ") + occurrences);

    for ( occurrences = 0, startPos = 10;
          startPos != -1 && startPos < test1.length();
          (startPos = test1.indexOf(test2, startPos)) != -1 ? (++occurrences, startPos += 4) : 0)
        ;
    if (occurrences != 4)
        errln(UnicodeString("indexOf with starting offset failed: "
                            "expected to find 4 occurrences, found ") + occurrences);

    int32_t endPos = 28;
    for ( occurrences = 0, startPos = 5;
          startPos != -1 && startPos < test1.length();
          (startPos = test1.indexOf(test2, startPos, endPos - startPos)) != -1 ? (++occurrences, startPos += 4) : 0)
        ;
    if (occurrences != 4)
        errln(UnicodeString("indexOf with starting and ending offsets failed: "
                            "expected to find 4 occurrences, found ") + occurrences);

    //using UChar32 string
    for ( startPos=0, occurrences=0;
          startPos != -1 && startPos < test3.length();
          (startPos = test3.indexOf(test4, startPos)) != -1 ? (++occurrences, startPos += 2) : 0)
        ;
    if (occurrences != 4)
        errln((UnicodeString)"indexOf failed: expected to find 4 occurrences, found " + occurrences);

    for ( startPos=10, occurrences=0;
          startPos != -1 && startPos < test3.length();
          (startPos = test3.indexOf(test4, startPos)) != -1 ? (++occurrences, startPos += 2) : 0)
        ;
    if (occurrences != 2)
        errln(UnicodeString("indexOf failed: expected to find 2 occurrences, found ") + occurrences);
    //---

    for ( occurrences = 0, startPos = 0;
          startPos != -1 && startPos < test1.length();
          (startPos = test1.indexOf(testChar, startPos)) != -1 ? (++occurrences, startPos += 1) : 0)
        ;
    if (occurrences != 16)
        errln(UnicodeString("indexOf with character failed: "
                            "expected to find 16 occurrences, found ") + occurrences);

    for ( occurrences = 0, startPos = 10;
          startPos != -1 && startPos < test1.length();
          (startPos = test1.indexOf(testChar, startPos)) != -1 ? (++occurrences, startPos += 1) : 0)
        ;
    if (occurrences != 12)
        errln(UnicodeString("indexOf with character & start offset failed: "
                            "expected to find 12 occurrences, found ") + occurrences);

    for ( occurrences = 0, startPos = 5, endPos = 28;
          startPos != -1 && startPos < test1.length();
          (startPos = test1.indexOf(testChar, startPos, endPos - startPos)) != -1 ? (++occurrences, startPos += 1) : 0)
        ;
    if (occurrences != 10)
        errln(UnicodeString("indexOf with character & start & end offsets failed: "
                            "expected to find 10 occurrences, found ") + occurrences);

    //testing for UChar32
    UnicodeString subString;
    for( occurrences =0, startPos=0; startPos < test3.length(); startPos +=1){
        subString.append(test3, startPos, test3.length());
        if(subString.indexOf(testChar32) != -1 ){
             ++occurrences;
        }
        subString.remove();
    }
    if (occurrences != 14)
        errln((UnicodeString)"indexOf failed: expected to find 14 occurrences, found " + occurrences);

    for ( occurrences = 0, startPos = 0;
          startPos != -1 && startPos < test3.length();
          (startPos = test3.indexOf(testChar32, startPos)) != -1 ? (++occurrences, startPos += 1) : 0)
        ;
    if (occurrences != 4)
        errln((UnicodeString)"indexOf failed: expected to find 4 occurrences, found " + occurrences);
     
    endPos=test3.length();
    for ( occurrences = 0, startPos = 5;
          startPos != -1 && startPos < test3.length();
          (startPos = test3.indexOf(testChar32, startPos, endPos - startPos)) != -1 ? (++occurrences, startPos += 1) : 0)
        ;
    if (occurrences != 3)
        errln((UnicodeString)"indexOf with character & start & end offsets failed: expected to find 2 occurrences, found " + occurrences);
    //---

    if(test1.lastIndexOf(test2)!=29) {
        errln("test1.lastIndexOf(test2)!=29");
    }

    if(test1.lastIndexOf(test2, 15)!=29 || test1.lastIndexOf(test2, 29)!=29 || test1.lastIndexOf(test2, 30)!=-1) {
        errln("test1.lastIndexOf(test2, start) failed");
    }

    for ( occurrences = 0, startPos = 32;
          startPos != -1;
          (startPos = test1.lastIndexOf(test2, 5, startPos - 5)) != -1 ? ++occurrences : 0)
        ;
    if (occurrences != 4)
        errln(UnicodeString("lastIndexOf with starting and ending offsets failed: "
                            "expected to find 4 occurrences, found ") + occurrences);

    for ( occurrences = 0, startPos = 32;
          startPos != -1;
          (startPos = test1.lastIndexOf(testChar, 5, startPos - 5)) != -1 ? ++occurrences : 0)
        ;
    if (occurrences != 11)
        errln(UnicodeString("lastIndexOf with character & start & end offsets failed: "
                            "expected to find 11 occurrences, found ") + occurrences);

    //testing UChar32
    startPos=test3.length();
    for ( occurrences = 0;
          startPos != -1;
          (startPos = test3.lastIndexOf(testChar32, 5, startPos - 5)) != -1 ? ++occurrences : 0)
        ;
    if (occurrences != 3)
        errln((UnicodeString)"lastIndexOf with character & start & end offsets failed: expected to find 3 occurrences, found " + occurrences);


    for ( occurrences = 0, endPos = test3.length();  endPos > 0; endPos -= 1){
        subString.remove();
        subString.append(test3, 0, endPos);
        if(subString.lastIndexOf(testChar32) != -1 ){
            ++occurrences;
        }
    }
    if (occurrences != 18)
        errln((UnicodeString)"indexOf failed: expected to find 18 occurrences, found " + occurrences);
    //---

    // test that indexOf(UChar32) and lastIndexOf(UChar32)
    // do not find surrogate code points when they are part of matched pairs
    // (= part of supplementary code points)
    // Jitterbug 1542
    if(test3.indexOf((UChar32)0xd841) != 4 || test3.indexOf((UChar32)0xdc02) != 3) {
        errln("error: UnicodeString::indexOf(UChar32 surrogate) finds a partial supplementary code point");
    }
    if( UnicodeString(test3, 0, 17).lastIndexOf((UChar)0xd841, 0) != 4 ||
        UnicodeString(test3, 0, 17).lastIndexOf((UChar32)0xd841, 2) != 4 ||
        test3.lastIndexOf((UChar32)0xd841, 0, 17) != 4 || test3.lastIndexOf((UChar32)0xdc02, 0, 17) != 16
    ) {
        errln("error: UnicodeString::lastIndexOf(UChar32 surrogate) finds a partial supplementary code point");
    }
}