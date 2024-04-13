int FindStartOffsetOfFileInZipFile(const char* zip_file, const char* filename) {
  FileDescriptor fd;
  if (!fd.OpenReadOnly(zip_file)) {
    LOG_ERRNO("%s: open failed trying to open zip file %s\n",
              __FUNCTION__, zip_file);
    return CRAZY_OFFSET_FAILED;
  }

  struct stat stat_buf;
  if (stat(zip_file, &stat_buf) == -1) {
    LOG_ERRNO("%s: stat failed trying to stat zip file %s\n",
              __FUNCTION__, zip_file);
    return CRAZY_OFFSET_FAILED;
  }

  if (stat_buf.st_size > kMaxZipFileLength) {
    LOG("%s: The size %ld of %s is too large to map\n",
        __FUNCTION__, stat_buf.st_size, zip_file);
    return CRAZY_OFFSET_FAILED;
  }

  void* mem = fd.Map(NULL, stat_buf.st_size, PROT_READ, MAP_PRIVATE, 0);
  if (mem == MAP_FAILED) {
    LOG_ERRNO("%s: mmap failed trying to mmap zip file %s\n",
              __FUNCTION__, zip_file);
    return CRAZY_OFFSET_FAILED;
  }
   ScopedMMap scoped_mmap(mem, stat_buf.st_size);
 
   uint8_t* mem_bytes = static_cast<uint8_t*>(mem);
  int off;
  for (off = stat_buf.st_size - sizeof(kEndOfCentralDirectoryMarker);
       off >= 0; --off) {
     if (ReadUInt32(mem_bytes, off) == kEndOfCentralDirectoryMarker) {
       break;
     }
  }
  if (off == -1) {
    LOG("%s: Failed to find end of central directory in %s\n",
        __FUNCTION__, zip_file);
    return CRAZY_OFFSET_FAILED;
  }


  uint32_t length_of_central_dir = ReadUInt32(
      mem_bytes, off + kOffsetOfCentralDirLengthInEndOfCentralDirectory);
  uint32_t start_of_central_dir = ReadUInt32(
      mem_bytes, off + kOffsetOfStartOfCentralDirInEndOfCentralDirectory);

  if (start_of_central_dir > off) {
    LOG("%s: Found out of range offset %u for start of directory in %s\n",
        __FUNCTION__, start_of_central_dir, zip_file);
    return CRAZY_OFFSET_FAILED;
  }

  uint32_t end_of_central_dir = start_of_central_dir + length_of_central_dir;
  if (end_of_central_dir > off) {
    LOG("%s: Found out of range offset %u for end of directory in %s\n",
        __FUNCTION__, end_of_central_dir, zip_file);
    return CRAZY_OFFSET_FAILED;
  }

  uint32_t num_entries = ReadUInt16(
      mem_bytes, off + kOffsetNumOfEntriesInEndOfCentralDirectory);

  off = start_of_central_dir;
  const int target_len = strlen(filename);
  int n = 0;
  for (; n < num_entries && off < end_of_central_dir; ++n) {
    uint32_t marker = ReadUInt32(mem_bytes, off);
    if (marker != kCentralDirHeaderMarker) {
      LOG("%s: Failed to find central directory header marker in %s. "
          "Found 0x%x but expected 0x%x\n", __FUNCTION__,
          zip_file, marker, kCentralDirHeaderMarker);
      return CRAZY_OFFSET_FAILED;
    }
    uint32_t file_name_length =
        ReadUInt16(mem_bytes, off + kOffsetFilenameLengthInCentralDirectory);
    uint32_t extra_field_length =
        ReadUInt16(mem_bytes, off + kOffsetExtraFieldLengthInCentralDirectory);
    uint32_t comment_field_length =
        ReadUInt16(mem_bytes, off + kOffsetCommentLengthInCentralDirectory);
    uint32_t header_length = kOffsetFilenameInCentralDirectory +
        file_name_length + extra_field_length + comment_field_length;

    uint32_t local_header_offset =
        ReadUInt32(mem_bytes, off + kOffsetLocalHeaderOffsetInCentralDirectory);

    uint8_t* filename_bytes =
        mem_bytes + off + kOffsetFilenameInCentralDirectory;

    if (file_name_length == target_len &&
        memcmp(filename_bytes, filename, target_len) == 0) {
      uint32_t marker = ReadUInt32(mem_bytes, local_header_offset);
      if (marker != kLocalHeaderMarker) {
        LOG("%s: Failed to find local file header marker in %s. "
            "Found 0x%x but expected 0x%x\n", __FUNCTION__,
            zip_file, marker, kLocalHeaderMarker);
        return CRAZY_OFFSET_FAILED;
      }

      uint32_t compression_method =
          ReadUInt16(
              mem_bytes,
              local_header_offset + kOffsetCompressionMethodInLocalHeader);
      if (compression_method != kCompressionMethodStored) {
        LOG("%s: %s is compressed within %s. "
            "Found compression method %u but expected %u\n", __FUNCTION__,
            filename, zip_file, compression_method, kCompressionMethodStored);
        return CRAZY_OFFSET_FAILED;
      }

      uint32_t file_name_length =
          ReadUInt16(
              mem_bytes,
              local_header_offset + kOffsetFilenameLengthInLocalHeader);
      uint32_t extra_field_length =
          ReadUInt16(
              mem_bytes,
              local_header_offset + kOffsetExtraFieldLengthInLocalHeader);
      uint32_t header_length =
          kOffsetFilenameInLocalHeader + file_name_length + extra_field_length;

      return local_header_offset + header_length;
    }

    off += header_length;
  }

  if (n < num_entries) {
    LOG("%s: Did not find all the expected entries in the central directory. "
        "Found %d but expected %d\n", __FUNCTION__, n, num_entries);
  }

  if (off < end_of_central_dir) {
    LOG("%s: There are %d extra bytes at the end of the central directory.\n",
        __FUNCTION__, end_of_central_dir - off);
  }

  LOG("%s: Did not find %s in %s\n", __FUNCTION__, filename, zip_file);
  return CRAZY_OFFSET_FAILED;
}
