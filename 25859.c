ZipFile::OpenStreamCounter::~OpenStreamCounter()
{
    /* If you hit this assertion, it means you've created a stream to read one of the items in the
       zipfile, but you've forgotten to delete that stream object before deleting the file..
       Streams can't be kept open after the file is deleted because they need to share the input
       stream that is managed by the ZipFile object.
    */
    jassert (numOpenStreams == 0);
}