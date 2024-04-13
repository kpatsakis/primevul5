    void writeFlagsAndSizes (OutputStream& target) const
    {
        target.writeShort (10); // version needed
        target.writeShort ((short) (1 << 11)); // this flag indicates UTF-8 filename encoding
        target.writeShort ((! symbolicLink && compressionLevel > 0) ? (short) 8 : (short) 0); //symlink target path is not compressed
        writeTimeAndDate (target, fileTime);
        target.writeInt ((int) checksum);
        target.writeInt ((int) (uint32) compressedSize);
        target.writeInt ((int) (uint32) uncompressedSize);
        target.writeShort (static_cast<short> (storedPathname.toUTF8().sizeInBytes() - 1));
        target.writeShort (0); // extra field length
    }