int get_bitindex(int row, int col) {
    // Map board coordinates to a bitboard index.
    return (7 - row) * 8 + col;
}