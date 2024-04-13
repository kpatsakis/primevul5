static void remove_all_media_directions(pjmedia_sdp_media *m)
{
    pjmedia_sdp_media_remove_all_attr(m, "inactive");
    pjmedia_sdp_media_remove_all_attr(m, "sendrecv");
    pjmedia_sdp_media_remove_all_attr(m, "sendonly");
    pjmedia_sdp_media_remove_all_attr(m, "recvonly");
}