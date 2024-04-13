ZipFile::ZipFile (InputStream& stream)  : inputStream (&stream)
{
    init();
}