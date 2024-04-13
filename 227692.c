static void boxes_check(size_t b,size_t m)
{
    if ( b > m ) {
#ifdef EXIV2_DEBUG_MESSAGES
        std::cout << "Exiv2::Jp2Image::readMetadata box maximum exceeded" << std::endl;
#endif
        throw Error(kerCorruptedMetadata);
    }
}