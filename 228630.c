void APar_TrackLevelInfo(Trackage *track, const char *track_search_atom_name) {
  uint8_t track_tally = 0;
  short iter = 0;

  while (parsedAtoms[iter].NextAtomNumber != 0) {

    if (strncmp(parsedAtoms[iter].AtomicName, "trak", 4) == 0) {
      track_tally += 1;
      if (track->track_num == 0) {
        track->total_tracks += 1;

      } else if (track->track_num == track_tally) {

        short next_atom = parsedAtoms[iter].NextAtomNumber;
        while (parsedAtoms[next_atom].AtomicLevel >
               parsedAtoms[iter].AtomicLevel) {

          if (strncmp(parsedAtoms[next_atom].AtomicName,
                      track_search_atom_name,
                      4) == 0) {

            track->track_atom = parsedAtoms[next_atom].AtomicNumber;
            return;
          } else {
            next_atom = parsedAtoms[next_atom].NextAtomNumber;
          }
          if (parsedAtoms[next_atom].AtomicLevel ==
              parsedAtoms[iter].AtomicLevel) {
            track->track_atom = 0;
          }
        }
      }
    }
    iter = parsedAtoms[iter].NextAtomNumber;
  }
  return;
}