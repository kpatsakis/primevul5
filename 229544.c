UnicodeStringTest::TestStackAllocation()
{
    UChar           testString[] ={ 
        0x54, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x61, 0x20, 0x63, 0x72, 0x61, 0x7a, 0x79, 0x20, 0x74, 0x65, 0x73, 0x74, 0x2e, 0 };
    UChar           guardWord = 0x4DED;
    UnicodeString*  test = 0;

    test = new  UnicodeString(testString);
    if (*test != "This is a crazy test.")
        errln("Test string failed to initialize properly.");
    if (guardWord != 0x04DED)
        errln("Test string initialization overwrote guard word!");

    test->insert(8, "only ");
    test->remove(15, 6);
    if (*test != "This is only a test.")
        errln("Manipulation of test string failed to work right.");
    if (guardWord != 0x4DED)
        errln("Manipulation of test string overwrote guard word!");

    // we have to deinitialize and release the backing store by calling the destructor
    // explicitly, since we can't overload operator delete
    delete test;

    UChar workingBuffer[] = {
        0x4e, 0x6f, 0x77, 0x20, 0x69, 0x73, 0x20, 0x74, 0x68, 0x65, 0x20, 0x74, 0x69, 0x6d, 0x65, 0x20,
        0x66, 0x6f, 0x72, 0x20, 0x61, 0x6c, 0x6c, 0x20, 0x6d, 0x65, 0x6e, 0x20, 0x74, 0x6f, 0x20,
        0x63, 0x6f, 0x6d, 0x65, 0xffff, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    UChar guardWord2 = 0x4DED;

    test = new UnicodeString(workingBuffer, 35, 100);
    if (*test != "Now is the time for all men to come")
        errln("Stack-allocated backing store failed to initialize correctly.");
    if (guardWord2 != 0x4DED)
        errln("Stack-allocated backing store overwrote guard word!");

    test->insert(24, "good ");
    if (*test != "Now is the time for all good men to come")
        errln("insert() on stack-allocated UnicodeString didn't work right");
    if (guardWord2 != 0x4DED)
        errln("insert() on stack-allocated UnicodeString overwrote guard word!");

    if (workingBuffer[24] != 0x67)
        errln("insert() on stack-allocated UnicodeString didn't affect backing store");

    *test += " to the aid of their country.";
    if (*test != "Now is the time for all good men to come to the aid of their country.")
        errln("Stack-allocated UnicodeString overflow didn't work");
    if (guardWord2 != 0x4DED)
        errln("Stack-allocated UnicodeString overflow overwrote guard word!");

    *test = "ha!";
    if (*test != "ha!")
        errln("Assignment to stack-allocated UnicodeString didn't work");
    if (workingBuffer[0] != 0x4e)
        errln("Change to UnicodeString after overflow are still affecting original buffer");
    if (guardWord2 != 0x4DED)
        errln("Change to UnicodeString after overflow overwrote guard word!");

    // test read-only aliasing with setTo()
    workingBuffer[0] = 0x20ac;
    workingBuffer[1] = 0x125;
    workingBuffer[2] = 0;
    test->setTo(TRUE, workingBuffer, 2);
    if(test->length() != 2 || test->charAt(0) != 0x20ac || test->charAt(1) != 0x125) {
        errln("UnicodeString.setTo(readonly alias) does not alias correctly");
    }

    UnicodeString *c=test->clone();

    workingBuffer[1] = 0x109;
    if(test->charAt(1) != 0x109) {
        errln("UnicodeString.setTo(readonly alias) made a copy: did not see change in buffer");
    }

    if(c->length() != 2 || c->charAt(1) != 0x125) {
        errln("clone(alias) did not copy the buffer");
    }
    delete c;

    test->setTo(TRUE, workingBuffer, -1);
    if(test->length() != 2 || test->charAt(0) != 0x20ac || test->charAt(1) != 0x109) {
        errln("UnicodeString.setTo(readonly alias, length -1) does not alias correctly");
    }

    test->setTo(FALSE, workingBuffer, -1);
    if(!test->isBogus()) {
        errln("UnicodeString.setTo(unterminated readonly alias, length -1) does not result in isBogus()");
    }
    
    delete test;
     
    test=new UnicodeString();
    UChar buffer[]={0x0061, 0x0062, 0x20ac, 0x0043, 0x0042, 0x0000};
    test->setTo(buffer, 4, 10);
    if(test->length() !=4 || test->charAt(0) != 0x0061 || test->charAt(1) != 0x0062 ||
        test->charAt(2) != 0x20ac || test->charAt(3) != 0x0043){
        errln((UnicodeString)"UnicodeString.setTo(UChar*, length, capacity) does not work correctly\n" + prettify(*test));
    }
    delete test;


    // test the UChar32 constructor
    UnicodeString c32Test((UChar32)0x10ff2a);
    if( c32Test.length() != U16_LENGTH(0x10ff2a) ||
        c32Test.char32At(c32Test.length() - 1) != 0x10ff2a
    ) {
        errln("The UnicodeString(UChar32) constructor does not work with a 0x10ff2a filler");
    }

    // test the (new) capacity constructor
    UnicodeString capTest(5, (UChar32)0x2a, 5);
    if( capTest.length() != 5 * U16_LENGTH(0x2a) ||
        capTest.char32At(0) != 0x2a ||
        capTest.char32At(4) != 0x2a
    ) {
        errln("The UnicodeString capacity constructor does not work with an ASCII filler");
    }

    capTest = UnicodeString(5, (UChar32)0x10ff2a, 5);
    if( capTest.length() != 5 * U16_LENGTH(0x10ff2a) ||
        capTest.char32At(0) != 0x10ff2a ||
        capTest.char32At(4) != 0x10ff2a
    ) {
        errln("The UnicodeString capacity constructor does not work with a 0x10ff2a filler");
    }

    capTest = UnicodeString(5, (UChar32)0, 0);
    if(capTest.length() != 0) {
        errln("The UnicodeString capacity constructor does not work with a 0x10ff2a filler");
    }
}