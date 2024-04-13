HttpStateData::doneWithServer() const
{
    return !Comm::IsConnOpen(serverConnection);
}