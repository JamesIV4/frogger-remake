# Frogger Remake

- The game is in `godot/`, written in native C#.
- Treat `reference/frogger.zip` as immutable ground truth. `tools/prepare_rom.py` checks the precise set before producing local binaries. Keep original binaries and local Ghidra projects ignored.
- Regenerate `godot/Scripts/Generated/` with `python tools/recompile.py`; do not hand-edit generated instruction bodies. Preserve cycle counts, unsigned wrapping, flags, register liveness and interrupt boundaries. Source maps and direct addresses are essential.
- The vendored JavaScript is a checksum-pinned reference and test oracle. Keep it unchanged. Its descriptive names can be wrong; verify against bytes, Ghidra and MAME.
- Run `tools/verify.ps1` after gameplay changes. Check both native-vs-reference and independent MAME fixtures. Never count speculative instruction decodes as recovered functions or a decompiler success flag as semantic proof.
- Keep intentional modernization outside recovered instruction bodies and switchable. Collision changes may affect only the documented road hook unless explicitly expanded.
- Model sources, rigs and animation recipes are in `art/`. Keep the whole starting bank open, grass low and flat, lighting neutral, color contrast strong, water continuous (no square cell pattern), and typography retro but readable.
- Check actual Godot captures after presentation changes. Use the `--screenshot=<absolute path>` game argument for a bounded capture. Launch unattended checks with hidden windows and do not steal desktop input.
- Keep Godot `.uid` and `.import` settings. Ignore caches, generated build output, credentials and private exports. Preserve unrelated user edits.
