# Chess Engine Module And Algorithm Guide

This document is the first stop for LLM-assisted engine work. It explains where the code lives, how board state is represented, and why each move type uses its particular bit operations.

## Board Model

The engine keeps two views of the same position:

- `ChessBoard::board[8][8]`: a human-readable square grid used for direct square lookup, FEN loading, display, and simple validation.
- `ChessBoard::bitboards[12]`: one `uint64_t` bitboard per piece type and color, used for fast move generation and attack detection.

`Bitboard` is an alias for `uint64_t`. One bit represents one square. A set bit means that square is occupied or attacked, depending on context.

Square indexing is defined by `get_bitindex(row, col)`:

```cpp
index = (7 - row) * 8 + col
bit = 1ULL << index
```

That means:

- `a1` is bit `0`
- `h1` is bit `7`
- `a8` is bit `56`
- `h8` is bit `63`

The array row coordinate is inverted from bitboard rank order: row `0` is rank 8, and row `7` is rank 1. This is why white pawn advances use left shifts and black pawn advances use right shifts.

## Module Map

- `ChessBoard.hpp` / `ChessBoard.cpp`: owns board state, FEN parsing, SAN/UCI move handling, legal move filtering, castling, en passant, promotion, check, mate, stalemate, and game status.
- `Bitboard.hpp`: defines `Bitboard` and magic-bitboard table structs.
- `BitOps.hpp` / `BitOps.cpp`: low-level bit helpers: bit scan, popcount, generated occupancies, random magic candidates.
- `Constants.hpp`: file masks, rank masks, initial piece bitboards, castling trigger squares, and shared constants.
- `Move.hpp` / `Move.cpp`: move container and coordinate conversion.
- `MoveValidator.hpp`: abstract interface for piece validators.
- `PawnValidator.*`: pawn attacks and pseudo-legal pawn moves.
- `KnightValidator.*`: precomputed knight attack masks.
- `BishopValidator.*`: bishop magic-bitboard attacks.
- `RookValidator.*`: rook magic-bitboard attacks.
- `KingValidator.*`: precomputed king attack masks.
- `FenMoveTester.*`: CSV-driven FEN plus SAN move tests.
- `PGNTestRunner.*`: PGN replay validation and performance reporting.
- `PerftTester.*`: legal move tree node counts for regression and performance testing.
- `GUIBoard.*` and `main.cpp`: SFML GUI path. Keep core engine behavior out of these files.
- `testing_main.cpp`: headless test executable entrypoint.

## Common Bit Operations

Most engine algorithms are built from a small set of operations:

- `1ULL << square`: make a single-square bitboard.
- `a | b`: union two sets, such as combining attacks.
- `a & b`: intersection, such as attacks that hit enemy pieces.
- `a & ~b`: remove blocked or friendly squares.
- `bb &= bb - 1`: clear the least significant set bit while iterating.
- `bitScanForward(bb)`: get the least significant set bit index.
- `popcount(bb)`: count set bits.
- `<< n` and `>> n`: move every set bit by a board offset.

The important hazard is file wraparound. For example, shifting `h4 << 1` would numerically become `a5`, which is not a legal east move. Validators prevent that by masking out edge files before shifting, such as `origin & ~Files::H`.

## Move Generation Pipeline

`ChessBoard::getAllMoves()` builds legal moves in stages:

1. Build aggregate friendly and enemy occupancy bitboards.
2. Iterate each friendly piece by repeatedly scanning and clearing set bits.
3. Ask the relevant validator for pseudo-legal destination masks.
4. Add castling destinations for kings when rights, occupancy, and attack rules allow it.
5. Convert destination masks into `Move` objects.
6. Filter moves that leave the king in check.
7. If currently in check, restrict legal replies to king moves, checker captures, or blocks.

The piece validators deliberately generate attacks quickly and locally. King safety is handled centrally by `ChessBoard`, because pins, discovered checks, castling, and check evasion all depend on the full board.

## Pawns

Pawn code lives in `PawnValidator.cpp`. Pawns are asymmetric, so the current side to move controls shift direction.

White attacks:

```cpp
((origin & ~Files::H) << 9) | ((origin & ~Files::A) << 7)
```

Black attacks:

```cpp
((origin & ~Files::A) >> 9) | ((origin & ~Files::H) >> 7)
```

Why:

- White moves toward larger bit indexes, so it shifts left.
- Black moves toward smaller bit indexes, so it shifts right.
- Diagonal captures are rank movement plus one file: `+9`, `+7`, `-7`, or `-9`.
- File masks stop a pawn on `a` or `h` from wrapping around the board.

Pawn quiet moves use the same orientation:

- White one-step: `origin << 8`
- Black one-step: `origin >> 8`
- One-step moves are ANDed with `~allPieces`, because pawns cannot advance into occupied squares.
- Two-step moves are allowed only from `RANK_2` for white or `RANK_7` for black, and only if both the one-step and final square are empty.

Captures use:

```cpp
attacks &= enemyPieces | enPassant;
```

That lets normal captures target enemy pieces while en passant targets the transient en-passant square from FEN or the last double pawn push. The actual captured pawn is not on the destination square, so `ChessBoard::movePieceInternal()` removes the pawn behind the en-passant target.

Promotions are applied in the board move code after a pawn reaches rank 8 for white or rank 1 for black. SAN and UCI promotion parsing accepts queen, rook, bishop, and knight.

## Knights

Knight code lives in `KnightValidator.cpp`. It precomputes a destination mask for each square at startup.

Knight offsets are:

- `<< 17`, `<< 15`, `<< 10`, `<< 6`
- `>> 17`, `>> 15`, `>> 10`, `>> 6`

Why:

- A knight moves two ranks and one file, or two files and one rank.
- With this bit layout, one rank is an offset of 8 and one file is an offset of 1.
- The combined offsets are therefore `16 + 1`, `16 - 1`, `8 + 2`, and `8 - 2`, in both directions.

Before each shift, the origin is masked against edge files so a knight does not wrap from one side of the board to the other. `getAttacks()` removes friendly pieces from the precomputed mask:

```cpp
attacks & ~friendlyPieces
```

## Kings

King code lives in `KingValidator.cpp`. Like knights, king attacks are precomputed for all 64 squares.

King offsets are:

- North/south: `<< 8`, `>> 8`
- East/west: `<< 1`, `>> 1`
- Diagonals: `<< 9`, `<< 7`, `>> 7`, `>> 9`

Why:

- The king moves one square in any direction.
- File masks prevent horizontal and diagonal wraparound.
- Rank masks prevent shifts that would leave the board.

The validator returns adjacent pseudo-legal squares minus friendly occupancy. `ChessBoard` then removes destinations attacked by the enemy. This split matters because a king's local move pattern is simple, but king safety requires enemy attacks from the whole board.

## Sliding Pieces

Bishops and rooks use magic bitboards. Queens combine both bishop-like and rook-like attacks.

Magic bitboards turn a blocker layout into an attack mask:

1. Build a relevant occupancy mask for the square.
2. Keep only relevant blockers: `occupancy &= mask`.
3. Multiply by a magic number.
4. Shift the high bits into a compact table index.
5. Return the precomputed attack mask for that blocker pattern.

The multiplication step intentionally mixes the blocker bits. A good magic number maps every relevant blocker pattern for that square to a unique table index. That avoids ray scanning during normal move generation.

## Bishops

Bishop code lives in `BishopValidator.cpp`.

`bishopOccupancyMask(square)` contains diagonal squares that can block a bishop, excluding outer board edges. Edge squares are excluded because they do not affect which squares before them are reachable; they are always the end of the ray.

`bishopAttacksOnTheFly(square, occupancy)` scans four diagonal directions:

- northeast
- northwest
- southeast
- southwest

It adds each square on the ray and stops after the first occupied square. The blocker square is included because a bishop may capture an enemy blocker, and friendly blockers are removed later by `attacks & ~friendlyPieces`.

## Rooks

Rook code lives in `RookValidator.cpp`.

`rookOccupancyMask(square)` contains file and rank blocker squares, excluding outer board edges for the same reason bishops do.

`rookAttacksOnTheFly(square, occupancy)` scans:

- north
- south
- east
- west

It includes the first blocker square, then stops. Normal move generation removes friendly blockers from the final attack mask.

## Queens

Queens do not have their own validator file. `ChessBoard` treats them as the union of rook and bishop movement:

```cpp
bishopAttacks | rookAttacks
```

This rule must be used everywhere attacks matter: normal moves, king safety, check detection, pins, and check evasion. A common regression is to include queen attacks in one path but not another.

## Castling

Castling is coordinated by `ChessBoard`, not the king validator, because it depends on state beyond the king's one-square attack mask.

The engine checks:

- castling rights from FEN or prior moves
- king on its expected start square
- rook availability through the castling right and board state
- empty transit and destination squares
- king is not currently in check
- king does not pass through an attacked square
- king does not land on an attacked square

Queenside castling has two different masks:

- Occupancy mask: `b`, `c`, and `d` files must be empty.
- Attack mask: only the king path, `c` and `d`, must be safe.

This distinction is important. A queenside rook may pass through `b1` or `b8`; the king never does, so an attack on `b1` or `b8` does not make castling illegal.

Castling rights are revoked when:

- a king moves
- a rook moves from its original square
- a rook on its original square is captured

## En Passant

FEN parsing stores the en-passant target square as a bitboard. Pawn move generation includes that square as a legal capture target if a pawn attacks it.

The destination square is empty during en passant, so `movePieceInternal()` has special capture handling:

- White captures en passant by moving to the target and removing the black pawn one rank behind it.
- Black captures en passant by moving to the target and removing the white pawn one rank behind it.

The move is then checked like any other legal move. This is required because en passant can expose a rook or queen line through the captured pawn's original square.

## Pins And Checks

Pins and check evasion are centralized in `ChessBoard`.

Sliding attack lanes come from bishop-like and rook-like ray logic. If a friendly piece is between the king and an enemy rook, bishop, or queen, moving that piece may expose check. Legal move filtering keeps only moves that preserve the block or capture the pinning piece when allowed.

Check handling depends on checker count:

- No check: all moves that keep the king safe are allowed.
- Single check: king moves, checker captures, and sliding-check blocks are allowed.
- Double check: only king moves are allowed.

Knight and pawn checks cannot be blocked, so non-king replies must capture the checker. Sliding checks can also be blocked on the lane between checker and king.

## SAN And UCI Moves

`ChessBoard::movePieceSAN()` parses PGN-style moves:

- strips check, mate, and annotation suffixes
- recognizes castling
- recognizes promotion
- uses destination square, piece type, capture marker, and disambiguation to find a legal source
- validates by trying the move and rejecting candidates that leave the king in check

`ChessBoard::movePieceUCI()` parses coordinate moves such as `e2e4` and promotion suffixes such as `e7e8q`.

Both paths should ultimately rely on the same board legality rules. When fixing a move rule, test both direct FEN/SAN cases and perft counts when possible.

## FEN

FEN loading initializes:

- piece placement
- side to move
- castling rights
- en-passant target
- halfmove clock
- fullmove number

The parser updates both the square array and piece bitboards. If one representation changes without the other, move generation and display will disagree.

## Perft

Perft counts every legal move sequence to a fixed depth. It is the best broad regression test for move generation because it exercises ordinary moves, captures, checks, castling, promotions, and en passant through legal move trees.

`PerftTester`:

- loads `test_perft/*.csv`
- skips rows deeper than `PERFT_MAX_DEPTH`
- recursively calls legal move generation and move application
- prints elapsed time for each case
- prints a root divide on failure so the first incorrect branch can be identified

Depth 1 and 2 are quick smoke tests. Depth 3 and 4 catch many rule bugs. Depth 5 is much slower and is better used as an intentional performance or correctness run.

## Testing Workflow

Use the headless runner for engine changes:

```powershell
.\scripts\run_tests_windows.ps1
```

Set perft depth in PowerShell:

```powershell
$env:PERFT_MAX_DEPTH = "4"
.\scripts\run_tests_windows.ps1
```

Or edit `test_config.env`:

```text
PERFT_MAX_DEPTH=4
```

Regenerate generated test data:

```powershell
python scripts/generate_fen_tests.py
python scripts/generate_perft_tests.py
```

When changing move legality:

1. Add a narrow FEN/SAN case for the exact rule.
2. Run the FEN suite.
3. Run perft to at least depth 3.
4. Use perft divide output to identify the first root move with a wrong count.
5. Update this document and `AGENTS.md` if the module map or workflow changes.

## Performance Notes

Bitboards are used because many chess questions are set operations:

- Which pieces occupy a line?
- Which squares does this piece attack?
- Which attacks hit the king?
- Which pieces are friendly or enemy?

Answering those with `uint64_t` operations is faster and simpler than scanning all 64 squares for every question. Sliding pieces are the expensive case, so magic bitboards precompute blocker-dependent attacks and turn runtime lookup into mask, multiply, shift, and table access.

The current magic numbers are generated during initialization. If startup time becomes a real concern, the next simple optimization is to persist known-good magic numbers as constants.
