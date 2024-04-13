ScanLineInputFile::Data::Data (int numThreads):
        partNumber(-1),
        memoryMapped(false)
{
    //
    // We need at least one lineBuffer, but if threading is used,
    // to keep n threads busy we need 2*n lineBuffers
    //

    lineBuffers.resize (max (1, 2 * numThreads));
}