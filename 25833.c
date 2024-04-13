ZipFile::ZipFile (const File& file)  : inputSource (new FileInputSource (file))
{
    init();
}