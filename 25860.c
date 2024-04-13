ZipFile::ZipFile (InputStream* stream, bool deleteStreamWhenDestroyed)
   : inputStream (stream)
{
    if (deleteStreamWhenDestroyed)
        streamToDelete.reset (inputStream);

    init();
}