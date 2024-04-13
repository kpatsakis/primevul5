static jas_stream_t *jpc_streamlist_get(jpc_streamlist_t *streamlist, int streamno)
{
	assert(streamno < streamlist->numstreams);
	return streamlist->streams[streamno];
}