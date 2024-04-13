static bool getCoverageFormat12(vector<uint32_t>& coverage, const uint8_t* data, size_t size) {
 const size_t kNGroupsOffset = 12;
 const size_t kFirstGroupOffset = 16;

     const size_t kGroupSize = 12;
     const size_t kStartCharCodeOffset = 0;
     const size_t kEndCharCodeOffset = 4;
     if (kFirstGroupOffset > size) {
         return false;
     }
     uint32_t nGroups = readU32(data, kNGroupsOffset);
    if (kFirstGroupOffset + nGroups * kGroupSize > size) {
         return false;
     }
     for (uint32_t i = 0; i < nGroups; i++) {
 uint32_t groupOffset = kFirstGroupOffset + i * kGroupSize;
 uint32_t start = readU32(data, groupOffset + kStartCharCodeOffset);
 uint32_t end = readU32(data, groupOffset + kEndCharCodeOffset);
        addRange(coverage, start, end + 1); // file is inclusive, vector is exclusive
 }
 return true;
}
