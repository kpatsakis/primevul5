static int order_lookups(const void *_otl1, const void *_otl2) {
    const OTLookup *otl1 = *(const OTLookup **) _otl1, *otl2 = *(const OTLookup **) _otl2;
return( otl1->lookup_index - otl2->lookup_index );
}