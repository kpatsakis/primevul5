CairoFont::matches(Ref &other) {
  return (other.num == ref.num && other.gen == ref.gen);
}