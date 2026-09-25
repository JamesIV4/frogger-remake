# Third-party sources

- **arcade-js**, Karl Stiefvater / contributors: https://github.com/qarl/arcade-js, pinned commit `9387bbe817befacc235cd8f9f4dd56c652172481`, GPL-3.0-only. The copied paths and their original SHA-256 hashes are in `vendor/arcade-js/UPSTREAM.json`. The Z80 decoder and reference tests are reused; the native generator and host are local work under the same license. Some upstream English variable descriptions are inaccurate (notably lives/time); the native core follows bytes and verified effects.
- **Ghidra**, NSA / contributors: https://github.com/NationalSecurityAgency/ghidra. Used locally for independent binary analysis; not bundled into the game.
- **MAME**, MAMEdev / contributors: https://github.com/mamedev/mame, version 0.289. Used locally to capture independent function fixtures and inspect the board wiring; not bundled. Hardware notes are grounded in `src/mame/galaxian/galaxian.cpp`.
- **Press Start 2P**, CodeMan38: https://github.com/google/fonts/tree/main/ofl/pressstart2p. SIL Open Font License; license included beside the font.
- **Silkscreen**, Jason Kottke / contributors: https://github.com/google/fonts/tree/main/ofl/silkscreen. SIL Open Font License; license included beside the font.
- **Godot Engine**: https://godotengine.org, MIT license. **Blender**: https://blender.org, GPL. The model generation workflow adapts the editable recipe / rig / validation approach from the user's NetHack PixelHack guide; no NetHack assets were copied.

The original Frogger game and ROM data remain their respective owners' work. Original binaries are user-provided and ignored by Git. This repository does not download or publish ROMs.
