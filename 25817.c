inline uint32 readUnalignedLittleEndianInt (const void* buffer)
{
    auto data = readUnaligned<uint32> (buffer);
    return ByteOrder::littleEndianInt (&data);
}