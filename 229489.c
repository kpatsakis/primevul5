    virtual const UnicodeString *snext(UErrorCode &status) {
        if(U_SUCCESS(status) && i<UPRV_LENGTHOF(testEnumStrings)) {
            unistr=UnicodeString(testEnumStrings[i++], "");
            return &unistr;
        }

        return NULL;
    }