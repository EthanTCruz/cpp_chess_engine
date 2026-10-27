# Chess rules audit ? 2026-10-26

The review found rule and state bugs despite the original 78 FEN cases and PGN file passing. The fixes below are implemented and covered by regression tests. Threefold repetition claims remain intentionally disabled; ordinary stalemate is implemented.

## Findings and fixes

| Area | Finding | Result |
| --- | --- | --- |
| Checkmate | Move completion credited the side that was checkmated. Some zero-destination map entries also prevented terminal detection. | Winner is the opponent of the immobilized checked side. Loaded terminal FENs are adjudicated too. |
| Check evasion and pins | Separate SAN trial validation and move-generation pin/check masks could disagree, particularly for en passant. | All entry points select from the same legal generator. Candidate occupancy is checked against king attacks, covering double check, discovered check, checker captures, interposition, and en passant. |
| En passant | Legal generation could miss the captured pawn disappearing or reject an en passant capture of a checking pawn. Targets could accumulate on FEN reset, and FEN output always wrote `-`. | Validate victim, rank, and empty target; remove victim during safety checks; expire rights after one reply; replace target on reset; serialize the actual target. |
| Castling | Rights alone could permit castling without a rook. Public castle-query helpers were incomplete or returned inverted masks. | Require king and rook on their starting squares, rights, empty path, and safe king start/transit/destination. Queenside b-file occupancy and attacks remain distinct. |
| Promotion | Coordinate/direct entry points allowed missing or misplaced promotions and insufficient input validation. | Only a pawn reaching its final rank may promote, with an explicit queen/rook/bishop/knight choice. The GUI now offers all four choices. Perft expands all four. |
| SAN | Ambiguous moves selected the first matching piece; one existing test explicitly expected this. | Require exactly one legal source. Pinned pseudo-candidates do not cause false ambiguity. Validate capture markers and promotion syntax. |
| Coordinates and FEN | Out-of-range coordinates could reach array indexing or shifts. Malformed FEN could corrupt placement. | Reject bad bounds and UCI lengths/suffixes; validate FEN fields and rank widths; replace FEN state atomically. Default fullmove number is 1. |
| Draws | No fifty-/seventy-five-move or material adjudication existed. All draws were conflated with stalemate. | Explicit fifty-move claims, including an intended move; automatic seventy-five-move draws; standard material draws; separate draw and stalemate flags. Checkmate takes precedence. |
| Repetition | No position history existed. | Automatic fivefold repetition uses placement, turn, castling rights, and legal en passant availability. Threefold claims are excluded as requested. |
| Resignation and finished games | Invalid tokens cleared results, final outcomes could be overwritten, and moves could continue. | Invalid tokens are no-ops; results are immutable; finished games reject moves. A declared winner with insufficient mating material produces a draw. |
| Attacks | `isAttacked()` always returned false. Aggregate attacks omitted enemy pieces defended by their own side. | Use reverse attack queries, including pinned attackers and defended occupied squares. Differential tests compare the complete enemy attack map. |
| PGN validation | Attached move numbers were skipped; comments, variations, and FEN starts were not handled; final results could mask checkmate errors. | Replay mainlines with attached numbers, comments, NAGs, nested variations, and FEN starts. Check header/movetext results and preserve already adjudicated outcomes. |
| Test execution | The Windows script ran an old executable after failed builds. | Configuration/build failures stop the script. A focused CTest entry is registered. |

## Validation

- Existing Lichess PGN collection and five new targeted PGN games pass.
- **91/91 FEN/SAN cases** pass; the source generator and generated CSV agree.
- **91/91 stateful rule checks** pass, including invalid-input state preservation, castling-right loss, both mate winners, stalemate, all promotions, claim boundaries, reset, and repetition identity.
- The broad Release perft run passed **61/61 cases**: existing positions through depth five and nine new positions through depth four. Reference positions include Kiwipete, rook/en-passant endgames, promotion stress positions, and double-check mate. A final Release run repeats all available cases through depth four.
- Deterministic python-chess comparison passes **24,708 positions**, **628,256 legal move destinations including promotion variants**, and **24,657 UCI/SAN transitions**. It compares full legal move sets, all six resulting FEN fields, enemy attack maps, material draws, and automatic outcomes. Seed: `20261026`.
- Headless Debug and Release builds, focused CTest, and the SFML 2.6.1 Release GUI build pass. GUI promotion interaction has not been manually exercised. Existing GUI warnings about Unicode checkmark characters remain.

Perft tests use `ChessBoard(fen, false)`: legal analysis must continue through material/repetition/move-count draws. Normal games adjudicate after loading and after every accepted move. Move generation itself has no result/history side effects.

Logs from this run are local ignored artifacts: `rules-release.log` (broad depth-five run), `rules-final.log` (final depth-four run), `rules-debug.log`, and `rules-differential.log`. Their `[PERF]` records provide timing evidence. The starting-position depth-three Debug count fell from approximately 704 ms in the initial run to 333 ms in a subsequent run. This is indicative only: concurrent builds and differing suite sizes prevent treating overall times as a controlled benchmark. FEN syntax validation and adjudication also add work to position loading.

## Remaining limits

**This is not a certification of every FIDE rule.** The requested threefold exception is implemented, but there are other limits that should remain visible:

1. **Arbitrary dead positions:** material checks cover the standard provable material cases, not every locked pawn fortress or forced sequence from which checkmate is impossible. No general reachability solver or tablebase was added. Side-specific resignation checks have the same conservative limitation.
2. **Time controls:** the application has no running clock or flag-fall controller. Tournament rules about time expiration, illegal-move penalties, touch-move, and arbiters are not modeled by the board engine.
3. **Input scope:** FEN validation checks syntax and rank widths rather than proving that a position is reachable from the initial position. Analysis mode deliberately supports compositions. SAN check/mate/annotation suffixes remain permissive; the actual board result is computed independently. PGN variations are skipped rather than validated, and nonterminal declared results represent external resignation/draw agreement.
4. **History:** FEN cannot encode prior occurrences. Loading/resetting a board starts fresh repetition history.
5. **Verification scope:** no exhaustive proof over all chess positions, GUI click-through, SFML 3 runtime test, sanitizer run, or Linux/macOS build was performed.

Rule reference: [FIDE Laws of Chess, effective 1 January 2023](https://handbook.fide.com/chapter/e012023), especially articles 3, 5, and 9. The independent test oracle is the already installed python-chess package; it is not a new engine dependency.

## Reproduce

```powershell
$env:PERFT_MAX_DEPTH = "3"
.\scripts\run_tests_windows.ps1
ctest --test-dir build-tests -C Debug --output-on-failure
cmake --build build-tests --target cpp_chess_engine_tests --config Release
python scripts/check_rules_python_chess.py
$env:PERFT_MAX_DEPTH = "5"
.\build-tests\Release\cpp_chess_engine_tests.exe
```

The default executable suite includes all FEN, PGN, perft, and stateful tests. `--rules` runs just the stateful checks. `docs/ENGINE.md` documents the updated APIs and move-generation flow.
