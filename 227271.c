HttpStateData::mayReadVirginReplyBody() const
{
    // TODO: Be more precise here. For example, if/when reading trailer, we may
    // not be doneWithServer() yet, but we should return false. Similarly, we
    // could still be writing the request body after receiving the whole reply.
    return !doneWithServer();
}