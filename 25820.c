inline uint16 readUnalignedLittleEndianShort (const void* buffer)
{
    auto data = readUnaligned<uint16> (buffer);
    return ByteOrder::littleEndianShort (&data);
}