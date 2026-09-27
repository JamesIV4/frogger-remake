#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <array>
#include <cmath>
#include <chrono>
#include <filesystem>
#include <algorithm>
#include <map>
#include <set>
#include <deque>
#include <memory>
#include <stdexcept>
#include <iomanip>
#include <optional>

#include "../../src/ArcadeSimulation.h"
#include "../../src/NativeSound.h"
#include "../../src/Generated/MainProgram.h"
#include "../../src/Generated/SoundProgram.h"

namespace fs = std::filesystem;

static std::vector<uint8_t> ReadFileBytes(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f.is_open()) throw std::runtime_error("Could not open file: " + path);
    return std::vector<uint8_t>((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
}

static std::string ReadFileText(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) throw std::runtime_error("Could not open file: " + path);
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

static void WriteFileText(const std::string& path, const std::string& text) {
    std::ofstream f(path);
    if (!f.is_open()) throw std::runtime_error("Could not write file: " + path);
    f << text;
}

static void Check(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}

// ---------------- Presentation helper math ---------------- //

struct ModelFootprints {
    static constexpr float FrogAlongX = 8.56170f;
    static constexpr float FrogAcrossRow = 6.89952f;
    static constexpr float RiverGatorLengthTiles = 2.274321f;
    static constexpr float RiverGatorWidthTiles = 1.100000f;
    static constexpr float RiverGatorFrontTiles = 1.150847f;
    static constexpr float RiverGatorSnoutTiles = 0.725847f;
    static constexpr float LogTopTiles = 0.436645f;
    static constexpr float SnakeBottomTiles = 0.021363f;
    static constexpr float LadyBottomTiles = -0.006061f;

    static ModelFootprint Vehicle(int lane) {
        switch (lane) {
            case 6: return {-13.09056, 13.30560, 6.07488}; // truck
            case 7: return {-7.79520, 8.29920, 6.07488};  // sport
            case 8: return {-8.29920, 7.79520, 6.07488};  // car
            case 9: return {-6.76704, 8.66880, 6.24960};  // dozer
            case 10: return {-8.87040, 6.76704, 6.07488}; // racecar
            default: throw std::out_of_range("lane");
        }
    }
};

struct GameOptions {
    bool modernCollision;
    bool perspective;
    bool follow;
    bool fullscreen;
    bool operator==(const GameOptions& o) const {
        return modernCollision == o.modernCollision && perspective == o.perspective &&
               follow == o.follow && fullscreen == o.fullscreen;
    }
};

struct GameDefaults {
    static constexpr int PreferencesVersion = 2;
    static GameOptions FromStored(int version, bool modern, bool perspective, bool follow, bool fullscreen) {
        return version >= PreferencesVersion ? GameOptions{modern, perspective, follow, fullscreen}
                                             : GameOptions{true, true, true, true};
    }
};

struct RiverGatorFit {
    float centerOffsetPixels;
    float widthScale;
    float lengthScale;
};

struct BoardVisuals {
    static constexpr float LeftEdge = 8.0f;
    static constexpr float RightEdge = 232.0f;
    static constexpr int RiverGatorVisiblePixels = 71;
    static constexpr int RiverGatorDangerPixels = 40;
    static constexpr int LadyFrogRow = 96;

    static RiverGatorZone RiverGatorContact(int frogX, int tipX) {
        int behind = (tipX - frogX + 256) & 255;
        if (behind < RiverGatorDangerPixels) return RiverGatorZone::Snout;
        if (behind <= RiverGatorVisiblePixels) return RiverGatorZone::Back;
        return RiverGatorZone::Outside;
    }

    static bool RiverGatorActive(const FrameState& state) {
        return state.At(0x83b7) >= 2 && (state.At(0x8150) & 1) != 0 && state.At(0x8101) != 0;
    }

    static float LadyFrogHeight(float scale) {
        return -0.18f + ModelFootprints::LogTopTiles - scale * ModelFootprints::LadyBottomTiles + 0.012f;
    }

    static RiverGatorFit FitRiverGator(int nativeWidth) {
        float lengthScale = 47.0f / (16.0f * ModelFootprints::RiverGatorLengthTiles);
        float widthScale = 14.0f / (16.0f * ModelFootprints::RiverGatorWidthTiles);
        float centerOffset = nativeWidth / 2.0f - 13.0f - 16.0f * ModelFootprints::RiverGatorFrontTiles * lengthScale;
        return {centerOffset, widthScale, lengthScale};
    }

    static bool IntersectsPlayfield(float center, float halfWidth) {
        return center + halfWidth >= LeftEdge && center - halfWidth <= RightEdge;
    }

    static float InterpolateByte(int previous, int current, float alpha) {
        int d = ((current - previous + 128) & 255) - 128;
        if (std::abs(d) > 8) return (float)current;
        float a = std::clamp(alpha, 0.0f, 1.0f);
        return previous + d * a;
    }

    static float Smooth(float t) {
        t = std::clamp(t, 0.0f, 1.0f);
        return t * t * (3.0f - 2.0f * t);
    }

    static float HomeGatorReveal(const FrameState& state, bool full, float fraction = 0.0f) {
        if (full) return 1.0f;
        return Smooth((state.At(0x8122) + std::clamp(fraction, 0.0f, 1.0f)) / 80.0f);
    }

    static int TileAt(const FrameState& state, float frogSpaceX, int frogSpaceRow, int columnOffset = 0) {
        int col = (frogSpaceRow / 8 + columnOffset) & 31;
        int raw = state.objects[col * 2];
        int scroll = ((raw >> 4) | (raw << 4)) & 255;
        int nativeY = (248 - (int)std::round(frogSpaceX) + scroll) & 255;
        return state.video[(nativeY >> 3) * 32 + col];
    }

    static int TurtlePhase(const FrameState& state, float x, int row) {
        bool surface = false, bubbles = false, blank = true;
        for (int offset : {-4, 0, 4}) {
            for (int col = 0; col < 2; col++) {
                int tile = TileAt(state, x + offset, row, col);
                surface |= (tile >= 0x70 && tile <= 0x87);
                bubbles |= (tile >= 0x94 && tile <= 0x9b);
                blank &= (tile == 0x10);
            }
        }
        return surface ? 0 : bubbles ? 1 : blank ? 2 : 0;
    }

    static float TurtleDepth(const FrameState& state, float x, int row, float alpha = 0.0f, bool knownDiver = false) {
        if (!knownDiver && TurtlePhase(state, x, row) == 0) return 0.0f;
        float p = state.At(row == 64 ? 0x8110 : 0x8111) + (row == 64 ? 1 : 2) * alpha;
        if (row == 64) {
            if (p < 80) return 0.0f;
            if (p < 160) return 0.70f * Smooth((p - 80) / 80.0f);
            if (p < 176) return 0.70f;
            return 0.70f * (1.0f - Smooth((p - 176) / 80.0f));
        }
        if (p < 80) return 0.70f * Smooth(p / 80.0f);
        if (p < 96) return 0.70f;
        if (p < 176) return 0.70f * (1.0f - Smooth((p - 96) / 80.0f));
        return 0.0f;
    }
};

class LadyFrogPresentation {
    bool hasLastX = false;
    float lastX = 0;
public:
    bool Active = false;
    float X = 0;
    bool Visible(float halfWidth) const {
        return Active && BoardVisuals::IntersectsPlayfield(X, halfWidth);
    }
    void Reset() { Active = false; hasLastX = false; X = 0; }
    void Observe(const FrameState& state, std::function<int(int)> readRom) {
        Active = state.At(0x83fe) != 0 && state.At(0x8135) != 0 && state.At(0x8134) == 0;
        if (!Active) { hasLastX = false; return; }
        int code = state.At(0x8041), color = state.At(0x8042), row = state.At(0x8043);
        bool patrolSprite = (code == 0x1e || code == 0x21 || code == 0xa1) && color == 4 && row == BoardVisuals::LadyFrogRow;
        int x;
        if (patrolSprite) x = state.At(0x8040);
        else {
            int index = (state.At(0x833d) & 0x7f) + 1;
            int offset = readRom(0x279f + index);
            x = offset >= 2 ? (offset + state.At(0x811c)) & 255 : (hasLastX ? (int)lastX : state.At(0x8040));
        }
        X = (float)x;
        lastX = (float)x;
        hasLastX = true;
    }
};

class HomeGatorVisual {
    float retreatStart = -1.0f;
    float lastReveal = 0.0f;
    bool wasActive = false;
public:
    static constexpr float RetreatFrames = 12.0f;
    bool Retreating() const { return retreatStart >= 0.0f; }
    void Reset() { retreatStart = -1.0f; lastReveal = 0.0f; wasActive = false; }
    std::optional<float> Reveal(float frame, bool active, float nativeReveal) {
        if (active) {
            wasActive = true;
            retreatStart = -1.0f;
            lastReveal = nativeReveal;
            return nativeReveal;
        }
        if (wasActive) {
            wasActive = false;
            retreatStart = frame;
        }
        if (retreatStart < 0.0f) return std::nullopt;
        float t = std::clamp((frame - retreatStart) / RetreatFrames, 0.0f, 1.0f);
        if (t >= 1.0f) {
            retreatStart = -1.0f;
            return std::nullopt;
        }
        float smooth = t * t * (3.0f - 2.0f * t);
        return lastReveal * (1.0f - smooth);
    }
};

class PresentationMotion {
    struct Track {
        float native;
        float display;
        float velocity = 0;
        int frame;
        std::deque<std::pair<int, float>> history;
        Track(float pos, int f) : native(pos), display(pos), frame(f) {
            history.push_back({f, pos});
        }
    };
    std::map<int, Track> tracks;
    int historyFrames;
    float contactTolerance;
    float correctionRate;

    static float Wrap(float x) {
        float rem = std::fmod(x, 256.0f);
        return rem < 0 ? rem + 256.0f : rem;
    }
public:
    PresentationMotion(int historyFrames = 32, float contactTolerance = 1.0f, float correctionRate = 24.0f)
        : historyFrames(std::max(2, historyFrames)),
          contactTolerance(std::clamp(contactTolerance, 0.25f, 2.0f)),
          correctionRate(std::clamp(correctionRate, 1.0f, 120.0f)) {}

    void Reset() { tracks.clear(); }
    float Step(int id, float nativeX, int nativeFrame, float renderSeconds, bool paused = false) {
        auto it = tracks.find(id);
        if (it == tracks.end()) {
            tracks.emplace(id, Track(nativeX, nativeFrame));
            return Wrap(nativeX);
        }
        Track& track = it->second;
        int elapsed = nativeFrame - track.frame;
        if (elapsed < 0 || elapsed > 24) {
            track = Track(nativeX, nativeFrame);
            return Wrap(nativeX);
        }
        if (elapsed > 0) {
            float advance = nativeX - track.native;
            advance -= 256.0f * std::round(advance / 256.0f);
            if (std::abs(advance) > 12.0f) {
                track = Track(nativeX, nativeFrame);
                return Wrap(nativeX);
            }
            track.native += advance;
            track.frame = nativeFrame;
            track.history.push_back({nativeFrame, track.native});
            while ((int)track.history.size() > historyFrames) track.history.pop_front();
            const auto& oldest = track.history.front();
            if (nativeFrame > oldest.first) {
                float desired = (track.native - oldest.second) / (nativeFrame - oldest.first);
                track.velocity += (desired - track.velocity) * (1.0f - std::exp(-0.55f * elapsed));
            }
        }
        if (!paused) {
            track.display += track.velocity * std::clamp(renderSeconds / (float)ArcadeSimulationCore::FrameSeconds, 0.0f, 1.5f);
        }
        float error = track.native - track.display;
        if (std::abs(error) > 4.0f) {
            track.display = track.native;
        } else if (std::abs(error) > contactTolerance) {
            float correction = error - std::copysign(contactTolerance, error);
            track.display += correction * std::clamp(renderSeconds * correctionRate, 0.0f, 1.0f);
        }
        if (track.display >= 512.0f || track.display < -256.0f) {
            float shift = 256.0f * std::floor(track.display / 256.0f);
            track.display -= shift;
            track.native -= shift;
            for (auto& item : track.history) item.second -= shift;
        }
        return Wrap(track.display);
    }
};

class FrogVisualState {
public:
    bool Dying = false;
    bool Drowning = false;
    bool Carrying = false;
    int DeathFrame = 0;
    int DeathX = 0;
    int DeathRow = 0;
    int HopDirection = 0;
    int HopStartFrame = 0;
    int previousHop = 0;

    static constexpr float MaximumDrownDepth = 1.15f;

    static bool PlayerOnBoard(const FrameState& s) {
        return s.At(0x8044) >= 8 && s.At(0x8044) <= 240 && s.At(0x8047) >= 26 && s.At(0x8047) <= 240;
    }

    void Observe(const FrameState& s) {
        bool dead = PlayerOnBoard(s) && s.At(0x8004) != 0 && s.At(0x83cd) == 0;
        if (dead && !Dying) {
            DeathFrame = s.frame;
            DeathX = s.At(0x8044);
            DeathRow = s.At(0x8047);
            Drowning = s.At(0x829c) != 0;
        }
        Dying = dead;
        Carrying = !dead && PlayerOnBoard(s) && s.At(0x8134) != 0 && s.At(0x8135) != 0;
        int hop = 0;
        for (int i = 0; i < 4; i++) if (s.At(0x8248 + i) != 0) hop = i + 1;
        if (hop != 0 && hop != previousHop) {
            HopDirection = hop;
            HopStartFrame = s.frame;
        }
        previousHop = hop;
    }

    bool HopActive(int frame) const {
        return !Dying && HopDirection != 0 && frame - HopStartFrame < 10;
    }

    double DeathSeconds(int frame, float fraction) const {
        return std::max(0.0, (double)(frame - DeathFrame) + fraction) * ArcadeSimulationCore::FrameSeconds;
    }

    static float SquashProgress(double seconds) {
        float p = (float)std::clamp(seconds / 0.11, 0.0, 1.0);
        return p * p * (3.0f - 2.0f * p);
    }

    static float DrownDepth(double seconds) {
        return MaximumDrownDepth * (1.0f - (float)std::exp(-std::max(0.0, seconds) / 0.25));
    }

    static float DrownHeight(float surfaceHeight, float initialHeight, double seconds) {
        float floor = surfaceHeight - MaximumDrownDepth;
        float remaining = std::max(0.0f, initialHeight - floor);
        return initialHeight - remaining * DrownDepth(seconds) / MaximumDrownDepth;
    }
};

class HomeArrivalVisual {
public:
    static constexpr double HoldSeconds = 0.25;
    static constexpr double HopCompletionSeconds = 6.0 * ArcadeSimulationCore::FrameSeconds;
    static constexpr double HopClipSeconds = 10.0 / 60.0;

    int startFrame = -1;
    int X = 0;
    int Row = 0;
    int TargetX = 0;
    bool Passenger = false;
    double startHopPoseSeconds = 0;

    void Reset() { startFrame = -1; Passenger = false; }
    void Cancel() { Reset(); }
    void Begin(const BonusAward& timeAward, bool passenger, double hopPoseSeconds = 0.0) {
        if (timeAward.Kind != BonusKind::Time) throw std::invalid_argument("Expected a home time award");
        startFrame = timeAward.Frame;
        X = timeAward.X;
        Row = timeAward.Row;
        Passenger = passenger;
        int bay = std::clamp((int)std::round((X - 24) / 48.0), 0, 4);
        TargetX = 24 + 48 * bay;
        startHopPoseSeconds = std::clamp(hopPoseSeconds, 0.0, HopClipSeconds);
    }

    double Elapsed(int frame, float fraction) const {
        return std::max(0.0, (double)(frame - startFrame) + fraction) * ArcadeSimulationCore::FrameSeconds;
    }

    double Completion() const { return Row > 32 ? HopCompletionSeconds : 0.0; }

    bool Active(int frame, float fraction = 0.0f) const {
        return startFrame >= 0 && frame >= startFrame && Elapsed(frame, fraction) < Completion() + HoldSeconds;
    }

    bool FinishingHop(int frame, float fraction = 0.0f) const {
        return Active(frame, fraction) && Elapsed(frame, fraction) < Completion();
    }

    float VisualRow(int frame, float fraction = 0.0f) const {
        if (Completion() == 0.0) return 32.0f;
        float p = (float)std::clamp(Elapsed(frame, fraction) / Completion(), 0.0, 1.0);
        return Row + (32.0f - Row) * p;
    }

    double HopPoseSeconds(int frame, float fraction = 0.0f) const {
        if (Completion() == 0.0) return 0.0;
        float p = (float)std::clamp(Elapsed(frame, fraction) / Completion(), 0.0, 1.0);
        return startHopPoseSeconds + (HopClipSeconds - startHopPoseSeconds) * p;
    }
};

class InputPulse {
    static constexpr int Up = 1, Down = 2, Left = 4, Right = 8;
    int analogMask = 0;
public:
    void Reset() { analogMask = 0; }
    int FromInputs(int digitalMask, float stickX, float stickY) {
        int previous = analogMask;
        analogMask = 0;
        if (stickY < ((previous & Up) != 0 ? -0.30f : -0.55f)) analogMask |= Up;
        if (stickY > ((previous & Down) != 0 ? 0.30f : 0.55f)) analogMask |= Down;
        if (stickX < ((previous & Left) != 0 ? -0.30f : -0.55f)) analogMask |= Left;
        if (stickX > ((previous & Right) != 0 ? 0.30f : 0.55f)) analogMask |= Right;
        return FromHeldMask(digitalMask | analogMask);
    }
    int FromHeldMask(int mask) {
        mask &= Up | Down | Left | Right;
        if ((mask & Up) != 0) return Up;
        if ((mask & Down) != 0) return Down;
        if ((mask & Left) != 0) return Left;
        if ((mask & Right) != 0) return Right;
        return 0;
    }
};

// ---------------- Feedback Tests ---------------- //

static std::unique_ptr<ArcadeSimulationCore> StartedSim(const std::vector<uint8_t>& rom) {
    auto s = std::make_unique<ArcadeSimulationCore>(rom);
    for (int f = 0; f < 310; f++) {
        int inp = (f >= 150 && f < 156) ? 16 : (f >= 230 && f < 236) ? 32 : 0;
        s->Step(inp);
    }
    return s;
}

static void RunFeedbackTests(const std::vector<uint8_t>& rom) {
    Check(GameDefaults::FromStored(0, false, false, false, false) == GameOptions{true, true, true, true},
          "older settings did not migrate to modern collision and fullscreen follow perspective");
    Check(GameDefaults::FromStored(GameDefaults::PreferencesVersion, false, false, false, false) == GameOptions{false, false, false, false},
          "explicit settings were not preserved after migration");

    Check(BoardVisuals::IntersectsPlayfield(-25, 46), "left long log culled too early");
    Check(BoardVisuals::IntersectsPlayfield(260, 46), "right long log culled too early");
    Check(!BoardVisuals::IntersectsPlayfield(-40, 46), "fully offscreen log was retained");
    Check(std::abs(BoardVisuals::InterpolateByte(255, 0, 0.5f) - 255.5f) < 0.001f, "byte wrap interpolates through the board");
    Check(std::abs(BoardVisuals::InterpolateByte(0, 255, 0.5f) + 0.5f) < 0.001f, "reverse wrap interpolates through the board");

    FrameState gatorPhase{};
    gatorPhase.ram[0x8122 - 0x8000] = 0;
    Check(BoardVisuals::HomeGatorReveal(gatorPhase, false) == 0.0f, "home gator did not begin behind the hedge");
    gatorPhase.ram[0x8122 - 0x8000] = 40;
    Check(std::abs(BoardVisuals::HomeGatorReveal(gatorPhase, false) - 0.5f) < 0.001f, "home gator emergence skipped the ROM midpoint");
    gatorPhase.ram[0x8122 - 0x8000] = 80;
    Check(BoardVisuals::HomeGatorReveal(gatorPhase, false) == 1.0f && BoardVisuals::HomeGatorReveal(gatorPhase, true) == 1.0f,
          "home gator did not finish at the ROM full tile");

    HomeGatorVisual homeGator;
    Check(!homeGator.Reveal(0, false, 0).has_value(), "inactive home gator rendered before its ROM tile");
    Check(homeGator.Reveal(80, true, 1.0f) == 1.0f, "home gator failed to reach the bay");
    Check(homeGator.Reveal(81, false, 0).value_or(0) == 1.0f && homeGator.Retreating(), "home gator vanished when the ROM tile cleared");
    float retreatMid = homeGator.Reveal(87, false, 0).value_or(0);
    Check(retreatMid > 0.45f && retreatMid < 0.55f, "home gator did not retreat smoothly");
    Check(!homeGator.Reveal(93, false, 0).has_value() && !homeGator.Retreating(), "home gator did not clear the bay promptly");

    PresentationMotion motion;
    Check(motion.Step(1, 0, 0, 1.0f / 60.0f) == 0.0f, "moving model did not initialize at native position");
    float moving = motion.Step(1, 1, 1, 1.0f / 120.0f);
    float settling = motion.Step(1, 1, 1, 1.0f / 120.0f);
    Check(moving > 0 && moving < 1 && settling > moving && settling < 1, "visual step was not subpixel or did not settle");
    motion.Step(2, 255, 0, 1.0f / 60.0f);
    Check(motion.Step(2, 0, 1, 1.0f / 60.0f) > 255.0f, "moving model teleported across the ROM byte wrap");
    motion.Reset();
    Check(motion.Step(1, 80, 2, 1.0f / 60.0f) == 80.0f, "motion reset retained a prior level position");

    auto movingBoard = StartedSim(rom);
    PresentationMotion laneMotion;
    int nativeStalls = 0, smoothedDuringStall = 0, nativeMoves = 0;
    float maxVisualLag = 0;
    std::map<int, int> lastNative;
    std::map<int, float> lastShown;
    for (int f = 0; f < 180; f++) {
        movingBoard->Step();
        for (int lane : {0, 1, 2, 3, 4, 6, 7, 8, 9, 10}) {
            int table = 0x8100 + lane * 9;
            if (movingBoard->Peek(table) == 0) continue;
            int raw = movingBoard->Peek(table + 1);
            laneMotion.Step(lane, (float)raw, f, 1.0f / 120.0f);
            float shown = laneMotion.Step(lane, (float)raw, f, 1.0f / 120.0f);
            float lag = raw - shown;
            lag -= 256.0f * std::round(lag / 256.0f);
            maxVisualLag = std::max(maxVisualLag, std::abs(lag));
            if (lastNative.find(lane) != lastNative.end()) {
                int old = lastNative[lane];
                if (old == raw) {
                    nativeStalls++;
                    if (std::abs(shown - lastShown[lane]) > 0.001f) smoothedDuringStall++;
                } else {
                    nativeMoves++;
                }
            }
            lastNative[lane] = raw;
            lastShown[lane] = shown;
        }
    }
    Check(nativeMoves > 0 && nativeStalls > 0 && smoothedDuringStall > 0,
          "actual ROM object motion did not receive subpixel presentation between native steps");
    Check(maxVisualLag < 2.5f, "smoothing placed moving models too far from their native collision positions");

    Check(FrogVisualState::DrownDepth(0.1) > 0.3f, "water death waits visibly above the surface");
    float surfaceDrown = 0.20f, divingStart = surfaceDrown - 0.70f;
    float finalDrown = surfaceDrown - FrogVisualState::MaximumDrownDepth;
    Check(std::abs(FrogVisualState::DrownHeight(surfaceDrown, divingStart, 0) - divingStart) < 0.0001f,
          "diving death popped to the surface at onset");
    float priorDrown = divingStart;
    for (int frame = 1; frame <= 90; frame++) {
        float height = FrogVisualState::DrownHeight(surfaceDrown, divingStart, frame * ArcadeSimulationCore::FrameSeconds);
        Check(height <= priorDrown + 0.0001f && height >= finalDrown - 0.0001f,
              "diving death rebounded or passed the surface-death floor");
        priorDrown = height;
    }
    Check(std::abs(FrogVisualState::DrownHeight(surfaceDrown, divingStart, 99) - FrogVisualState::DrownHeight(surfaceDrown, surfaceDrown, 99)) < 0.0001f,
          "turtle and surface drowning finished at different depths");

    struct DeathRecord { bool water; int frames; int subcounterResets; bool monotonic; };
    std::vector<DeathRecord> deaths;
    for (bool water : {false, true}) {
        auto sim = StartedSim(rom);
        FrogVisualState view;
        sim->Poke(0x8044, 120);
        sim->Poke(0x8047, water ? 112 : 208);
        sim->Poke(0x8004, 1);
        sim->Poke(0x829c, water ? 1 : 0);
        sim->Poke(0x83cd, 0);
        double previousAge = -1;
        float previousDepth = -1, previousCompression = -1;
        int resets = 0, lastCounter = -1, frames = 0, anchorX = 0, anchorRow = 0;
        for (int n = 0; n < 100; n++) {
            sim->Step();
            auto state = sim->Snapshot();
            view.Observe(state);
            if (!view.Dying) {
                if (frames > 0) break;
                continue;
            }
            double age = view.DeathSeconds(state.frame, 0.0f);
            Check(age > previousAge, "death clock restarted at a ROM subphase");
            Check(view.Drowning == water, "death cause changed mid-animation");
            float depth = FrogVisualState::DrownDepth(age);
            float compression = FrogVisualState::SquashProgress(age);
            Check(depth >= previousDepth && compression >= previousCompression, "death geometry rebounded upward");
            previousDepth = depth;
            previousCompression = compression;
            if (frames == 0) {
                anchorX = state.At(0x8044);
                anchorRow = state.At(0x8047);
            }
            Check(view.DeathX == anchorX && view.DeathRow == anchorRow, "death anchor drifted");
            if (state.At(0x8247) < lastCounter) resets++;
            lastCounter = state.At(0x8247);
            previousAge = age;
            frames++;
        }
        Check(resets >= 3, "test never crossed the resetting ROM counter");
        deaths.push_back({water, frames, resets, true});
    }

    struct BonusTestRecord { bool bug; bool rider; bool fifth; int events; };
    std::vector<BonusTestRecord> bonuses;
    std::tuple<bool, bool, bool> bonusCases[] = {
        {false, false, false}, {true, false, false}, {false, true, false}, {true, true, false}, {true, true, true}
    };
    for (auto [bug, rider, fifth] : bonusCases) {
        auto sim = StartedSim(rom);
        int bay = fifth ? 5 : 1;
        if (fifth) {
            for (int i = 0; i < 4; i++) sim->Poke(0x825e + i, 1);
            sim->Poke(0x825c, 4);
        }
        sim->Poke(0x8044, 24 + 48 * (bay - 1));
        sim->Poke(0x8047, 32);
        sim->Poke(0x8004, 0);
        sim->Poke(0x83cd, 0);
        sim->Poke(0x8120, 0);
        sim->Poke(0x8121, bug ? bay : 0);
        sim->Poke(0x8122, 1);
        sim->Poke(0x8134, rider ? 1 : 0);
        sim->Poke(0x8135, rider ? 1 : 0);
        FrogVisualState carried;
        carried.Observe(sim->Snapshot());
        Check(carried.Carrying == rider, "carried frog flag was not exposed");
        int before = sim->Snapshot().BcdScore(0x83ed);
        for (int f = 0; f < 10; f++) sim->Step();
        const auto& awards = sim->BonusAwards;
        int expected = (bug ? 1 : 0) + (rider ? 1 : 0) + 1;
        Check((int)awards.size() == expected, "bonus call count mismatch");
        int bugCount = 0, riderCount = 0, timeCount = 0, sumAmount = 0;
        BonusAward timeAward{};
        for (const auto& a : awards) {
            if (a.Kind == BonusKind::Bug) bugCount++;
            if (a.Kind == BonusKind::Rescue) riderCount++;
            if (a.Kind == BonusKind::Time) { timeCount++; timeAward = a; }
            sumAmount += a.Amount;
            Check(a.Kind == BonusKind::Time || a.Amount == 200, "bonus amount changed");
            Check(a.X == 24 + 48 * (bay - 1), "bonus origin changed");
        }
        Check(bugCount == (bug ? 1 : 0), "bug bonus missing/duplicated");
        Check(riderCount == (rider ? 1 : 0), "rescue bonus missing/duplicated");
        Check(timeCount == 1, "time bonus missing/duplicated");
        Check(sim->Snapshot().BcdScore(0x83ed) - before >= sumAmount, "popup had no matching score award");

        HomeArrivalVisual hold;
        hold.Begin(timeAward, rider);
        Check(hold.X == timeAward.X && hold.Row == timeAward.Row && hold.Passenger == rider,
              "home hold lost arrival pose or rider");
        Check(hold.Active(timeAward.Frame) && hold.Active(timeAward.Frame + 15) && !hold.Active(timeAward.Frame + 16),
              "home hold is not a quarter second");
        hold.Cancel();
        Check(!hold.Active(timeAward.Frame + 1), "input did not cancel the home hold");
        bonuses.push_back({bug, rider, fifth, (int)awards.size()});
    }

    {
        auto arrival = StartedSim(rom);
        for (int bay = 0; bay < 4; bay++) arrival->Poke(0x825e + bay, 1);
        arrival->Poke(0x825c, 4);
        arrival->Poke(0x8044, 216);
        arrival->Poke(0x8047, 42);
        arrival->Poke(0x8004, 0);
        arrival->Poke(0x83cd, 0);
        arrival->Poke(0x8268, 0);
        for (int addr = 0x8248; addr <= 0x8253; addr++) arrival->Poke(addr, 0);
        arrival->Poke(0x8249, 1);
        arrival->Poke(0x8251, 5);
        arrival->Poke(0x8254, 2);
        arrival->Poke(0x8134, 1);
        arrival->Poke(0x8135, 1);

        std::stringstream arrivalTrace;
        arrivalTrace << "[\n";
        for (int frame = 0; frame < 18; frame++) {
            arrival->Step();
            auto snap = arrival->Snapshot();
            arrivalTrace << "  {\n"
                         << "    \"frame\": " << frame << ",\n"
                         << "    \"row\": " << snap.At(0x8047) << ",\n"
                         << "    \"x\": " << snap.At(0x8044) << ",\n"
                         << "    \"upActive\": " << snap.At(0x8249) << ",\n"
                         << "    \"upCounter\": " << snap.At(0x8251) << ",\n"
                         << "    \"bonus\": [\n";
            const auto& aw = arrival->BonusAwards;
            for (size_t i = 0; i < aw.size(); i++) {
                arrivalTrace << "      {\"Kind\": " << (int)aw[i].Kind << ", \"Row\": " << aw[i].Row << ", \"X\": " << aw[i].X << "}"
                             << (i + 1 < aw.size() ? ",\n" : "\n");
            }
            arrivalTrace << "    ]\n  }" << (frame < 17 ? ",\n" : "\n");
        }
        arrivalTrace << "]\n";
        WriteFileText("docs/evidence/home-hop-trace.json", arrivalTrace.str());

        BonusAward lastHomeAward{};
        for (const auto& a : arrival->BonusAwards) {
            if (a.Kind == BonusKind::Time) lastHomeAward = a;
        }
        Check(lastHomeAward.Row == 40 && lastHomeAward.X == 216, "final-home ROM award did not occur mid-hop at row 40");
        HomeArrivalVisual finish;
        finish.Begin(lastHomeAward, true, 4.0 / 60.0);
        int awarded = lastHomeAward.Frame;
        Check(finish.FinishingHop(awarded) && finish.VisualRow(awarded) == 40.0f, "final-home animation skipped its remaining hop");
        Check(finish.VisualRow(awarded + 3) < 40.0f && finish.VisualRow(awarded + 3) > 32.0f, "final-home hop did not pass through the bay");
        Check(!finish.FinishingHop(awarded + 6) && std::abs(finish.VisualRow(awarded + 6) - 32.0f) < 0.001f, "final-home hop did not land in the bay");
        Check(finish.Active(awarded + 21) && !finish.Active(awarded + 22), "final-home hold did not begin after landing");
        Check(finish.HopPoseSeconds(awarded + 3) > 4.0 / 60.0 && finish.HopPoseSeconds(awarded + 3) < 10.0 / 60.0,
              "final-home hop clip did not finish its remaining frames");
    }

    auto turtles = StartedSim(rom);
    struct TurtlePrior { float depth; int phase; int clock; };
    std::map<std::pair<int, int>, TurtlePrior> priorTurtles;
    float maxStep = 0.0f;
    std::set<std::pair<int, int>> knownDiving;
    std::stringstream turtleJumpsJson;
    int jumpCount = 0;
    for (int f = 0; f < 1200; f++) {
        turtles->Step();
        auto s = turtles->Snapshot();
        for (int lane : {1, 4}) {
            int count = std::min(8, s.At(0x8100 + lane * 9));
            for (int index = 0; index < count; index++) {
                float x = s.At(0x8101 + lane * 9 + index) - 12 - (lane == 1 ? 31 : 47) / 2.0f;
                int row = (lane + 3) * 16;
                int phase = BoardVisuals::TurtlePhase(s, x, row);
                int clock = s.At(lane == 1 ? 0x8110 : 0x8111);
                if (phase > 0) knownDiving.insert({lane, index});
                float depth = BoardVisuals::TurtleDepth(s, x, row, 0.0f, knownDiving.find({lane, index}) != knownDiving.end());
                auto key = std::make_pair(lane, index);
                auto it = priorTurtles.find(key);
                if (it != priorTurtles.end()) {
                    float delta = std::abs(depth - it->second.depth);
                    maxStep = std::max(maxStep, delta);
                    if (delta > 0.13f && jumpCount < 8) {
                        if (jumpCount > 0) turtleJumpsJson << ",\n";
                        turtleJumpsJson << "    {\"frame\": " << s.frame << ", \"lane\": " << lane << ", \"index\": " << index
                                        << ", \"oldDepth\": " << it->second.depth << ", \"newDepth\": " << depth
                                        << ", \"oldPhase\": " << it->second.phase << ", \"phase\": " << phase
                                        << ", \"oldClock\": " << it->second.clock << ", \"clock\": " << clock << "}";
                        jumpCount++;
                    }
                }
                priorTurtles[key] = {depth, phase, clock};
            }
        }
    }
    Check(maxStep < 0.13f, "turtle dive jumps exceeded tolerance");

    {
        auto right = StartedSim(rom);
        std::stringstream hopTraceJson;
        hopTraceJson << "[\n";
        for (int f = 0; f < 96; f++) {
            right->Step(f < 80 ? 8 : 0);
            auto s = right->Snapshot();
            hopTraceJson << "  {\"frame\": " << f << ", \"x\": " << s.At(0x8044) << ", \"row\": " << s.At(0x8047) << ", \"flags\": ["
                         << s.At(0x8248) << "," << s.At(0x8249) << "," << s.At(0x824a) << "," << s.At(0x824b) << "], \"counters\": ["
                         << s.At(0x8250) << "," << s.At(0x8251) << "," << s.At(0x8252) << "," << s.At(0x8253) << "]}"
                         << (f < 95 ? ",\n" : "\n");
        }
        hopTraceJson << "]\n";
        WriteFileText("docs/evidence/hop-trace.json", hopTraceJson.str());
    }

    auto held = StartedSim(rom);
    int heldStartX = held->Peek(0x8044), heldInputFrames = 0;
    InputPulse heldInput;
    FrogVisualState hopView;
    int lastActive = -1, landedFrames = 0;
    PresentationMotion frogMotion(4, 0.35f, 72.0f);
    int frogNativeStalls = 0, frogVisualMovesDuringStalls = 0, lastFrogX = -1;
    float lastFrogShown = 0, maxFrogLag = 0;
    for (int f = 0; f < 96; f++) {
        int button = heldInput.FromHeldMask(8);
        if (button != 0) heldInputFrames++;
        held->Step(button);
        auto s = held->Snapshot();
        hopView.Observe(s);
        int nativeX = s.At(0x8044);
        frogMotion.Step(0, (float)nativeX, s.frame, 1.0f / 120.0f);
        float shown = frogMotion.Step(0, (float)nativeX, s.frame, 1.0f / 120.0f);
        float lag = nativeX - shown;
        lag -= 256.0f * std::round(lag / 256.0f);
        maxFrogLag = std::max(maxFrogLag, std::abs(lag));
        if (lastFrogX == nativeX) {
            frogNativeStalls++;
            if (std::abs(shown - lastFrogShown) > 0.001f) frogVisualMovesDuringStalls++;
        }
        lastFrogX = nativeX;
        lastFrogShown = shown;
        bool nativeHop = (s.At(0x8248) != 0 || s.At(0x8249) != 0 || s.At(0x824a) != 0 || s.At(0x824b) != 0);
        if (nativeHop) lastActive = f;
        if (!nativeHop && hopView.HopActive(s.frame) && lastActive >= 0 && f - lastActive <= 2) landedFrames++;
    }
    int singlePressX = held->Peek(0x8044);
    Check(heldInputFrames == 96 && singlePressX == heldStartX + 16, "holding right triggered more than one hop");
    for (int f = 0; f < 8; f++) held->Step(heldInput.FromHeldMask(0));
    for (int f = 0; f < 24; f++) held->Step(heldInput.FromHeldMask(8));
    Check(held->Peek(0x8044) == singlePressX + 16, "release and new press did not produce exactly one more hop");

    InputPulse overlapInput;
    Check(overlapInput.FromHeldMask(8) == 8 && overlapInput.FromHeldMask(9) == 1 &&
          overlapInput.FromHeldMask(8) == 8 && overlapInput.FromHeldMask(0) == 0 &&
          overlapInput.FromHeldMask(8) == 8, "direction priority changed");

    InputPulse stickInput;
    Check(stickInput.FromInputs(0, 0.75f, 0) == 8 && stickInput.FromInputs(0, 0.4f, 0) == 8 &&
          stickInput.FromInputs(0, 0.72f, 0) == 8 && stickInput.FromInputs(0, 0.15f, 0) == 0 &&
          stickInput.FromInputs(0, 0.75f, 0) == 8, "stick hysteresis failed to hold until neutral");

    Check(landedFrames >= 2, "visual hop lost its landing frames when ROM flag cleared");
    Check(frogNativeStalls > 0 && frogVisualMovesDuringStalls > 0 && maxFrogLag < 2.5f,
          "frog presentation still follows raw pixel steps or lags input");

    auto bottom = StartedSim(rom);
    InputPulse bottomInput;
    for (int f = 0; f < 20; f++) bottom->Step(bottomInput.FromHeldMask(2));
    Check(bottom->Peek(0x8047) == 240 && FrogVisualState::PlayerOnBoard(bottom->Snapshot()), "ROM lower grass row is not visible or walkable");
    int bottomX = bottom->Peek(0x8044);
    for (int f = 0; f < 8; f++) bottom->Step(bottomInput.FromHeldMask(0));
    for (int f = 0; f < 30; f++) bottom->Step(bottomInput.FromHeldMask(8));
    Check(bottom->Peek(0x8044) > bottomX, "frog cannot traverse the lower grass row");

    auto riverFit = BoardVisuals::FitRiverGator(60);
    float nativeTip = 162.0f, rawCenter = nativeTip - 12.0f - 60.0f / 2.0f;
    float visualTip = rawCenter + riverFit.centerOffsetPixels + 16.0f * ModelFootprints::RiverGatorFrontTiles * riverFit.lengthScale;
    float visualSnout = 16.0f * ModelFootprints::RiverGatorSnoutTiles * riverFit.lengthScale;
    Check(std::abs(visualTip - (nativeTip - 25.0f)) < 0.01f && std::abs(visualSnout - 15.0f) < 0.01f,
          "river gator back extends into the ROM head kill interval");
    Check(std::abs(16.0f * ModelFootprints::RiverGatorLengthTiles * riverFit.lengthScale - 47.0f) < 0.01f &&
          std::abs(16.0f * ModelFootprints::RiverGatorWidthTiles * riverFit.widthScale - 14.0f) < 0.01f,
          "river gator no longer fits its ROM log slot");

    float ladyScale = 0.62f, logTop = -0.18f + ModelFootprints::LogTopTiles;
    Check(std::abs(BoardVisuals::LadyFrogHeight(ladyScale) + ModelFootprints::LadyBottomTiles * ladyScale - logTop - 0.012f) < 0.001f,
          "pink frog is not seated on the inset log");

    FrameState hiddenLady{};
    hiddenLady.ram[0x83fe - 0x8000] = 1;
    hiddenLady.ram[0x8135 - 0x8000] = 1;
    hiddenLady.ram[0x8040 - 0x8000] = 0;
    hiddenLady.ram[0x8041 - 0x8000] = 0;
    hiddenLady.ram[0x8043 - 0x8000] = 0;
    hiddenLady.ram[0x811c - 0x8000] = 100;
    hiddenLady.ram[0x833d - 0x8000] = 1;
    float ladyHalfWidth = ModelFootprints::FrogAlongX * ladyScale;
    LadyFrogPresentation ladyPresentation;
    ladyPresentation.Observe(hiddenLady, [&](int a){ return rom[a]; });
    Check(ladyPresentation.Visible(ladyHalfWidth) && ladyPresentation.X == 80.0f,
          "armed pink frog vanished or shifted when ROM erased its descriptor");
    hiddenLady.ram[0x8041 - 0x8000] = 0x19;
    hiddenLady.ram[0x8040 - 0x8000] = 216;
    ladyPresentation.Observe(hiddenLady, [&](int a){ return rom[a]; });
    Check(ladyPresentation.Visible(ladyHalfWidth) && ladyPresentation.X == 80.0f,
          "goal popup stole the pink frog's visual position");
    hiddenLady.ram[0x8041 - 0x8000] = 0;
    hiddenLady.ram[0x811c - 0x8000] = 25;
    ladyPresentation.Observe(hiddenLady, [&](int a){ return rom[a]; });
    Check(ladyPresentation.Visible(ladyHalfWidth) && ladyPresentation.X == 5.0f,
          "pink frog disappears while partly across the board edge");
    hiddenLady.ram[0x811c - 0x8000] = 21;
    ladyPresentation.Observe(hiddenLady, [&](int a){ return rom[a]; });
    Check(!ladyPresentation.Visible(ladyHalfWidth) && ladyPresentation.X == 1.0f,
          "off-board pink frog still rendered");
    hiddenLady.ram[0x8040 - 0x8000] = 120;
    hiddenLady.ram[0x8134 - 0x8000] = 1;
    ladyPresentation.Observe(hiddenLady, [&](int a){ return rom[a]; });
    Check(!ladyPresentation.Visible(ladyHalfWidth), "rescued pink frog duplicated on the log");
    hiddenLady.ram[0x8134 - 0x8000] = 0;
    hiddenLady.ram[0x8135 - 0x8000] = 0;
    ladyPresentation.Observe(hiddenLady, [&](int a){ return rom[a]; });
    Check(!ladyPresentation.Visible(ladyHalfWidth), "unarmed pink frog still rendered");

    for (int lane = 6; lane <= 10; lane++) {
        auto footprint = ModelFootprints::Vehicle(lane);
        double leftEdge = footprint.MinAlongX - ModelFootprints::FrogAlongX;
        double rightEdge = footprint.MaxAlongX + ModelFootprints::FrogAlongX;
        Check(ArcadeSimulationCore::ModelContact(leftEdge + 0.001, 176, leftEdge + 0.001, 176, 0, 0, 176, footprint) &&
              ArcadeSimulationCore::ModelContact(rightEdge - 0.001, 176, rightEdge - 0.001, 176, 0, 0, 176, footprint),
              "missing contact at authored front/back edge");
        Check(!ArcadeSimulationCore::ModelContact(leftEdge - 0.2, 176, leftEdge - 0.2, 176, 0, 0, 176, footprint) &&
              !ArcadeSimulationCore::ModelContact(rightEdge + 0.2, 176, rightEdge + 0.2, 176, 0, 0, 176, footprint),
              "vehicle collision exceeds model at front/back");
    }

    auto carFootprint = ModelFootprints::Vehicle(8);
    Check(ArcadeSimulationCore::ModelContact(-40, 160, 40, 160, 0, 0, 160, carFootprint), "frog swept through car");
    Check(ArcadeSimulationCore::ModelContact(0, 160, 0, 160, -40, 40, 160, carFootprint), "moving car swept through frog");
    Check(!ArcadeSimulationCore::ModelContact(-40, 190, 40, 190, 0, 0, 160, carFootprint), "collision ignores model row separation");

    // VerifyOriginalRiverGator
    struct CrocProbe { int x; int hold; int drown; int rideTile; bool visualDying; };
    auto Probe = [&](int x, bool modern) -> CrocProbe {
        ArcadeSimulationCore game(rom, modern);
        game.Poke(0x8150, 1); game.Poke(0x83b7, 2);
        game.Poke(0x8044, x); game.Poke(0x8047, 48); game.Poke(0x8101, 160);
        if (!modern || !game.ResolveModernRiverGator()) {
            game.Cpu.PC = 0x28bb; game.Cpu.SP = 0x87f0;
            game.Poke(0x87f0, 0x34); game.Poke(0x87f1, 0x12);
            int guard = 0;
            while (game.Cpu.PC != 0x1234 && ++guard < 1000) game.Cpu.StepInstruction();
            Check(game.Cpu.PC == 0x1234, "native crocodile test did not return");
        }
        FrogVisualState visual;
        visual.Observe(game.Snapshot());
        return {x, game.Peek(0x8004), game.Peek(0x829c), game.Peek(0xa846), visual.Dying};
    };
    std::vector<CrocProbe> cases, modernCases;
    for (int x : {90, 91, 103, 110, 120, 121, 130, 144, 145, 152, 160, 161}) {
        cases.push_back(Probe(x, false));
        modernCases.push_back(Probe(x, true));
        for (const auto& probe : {cases.back(), modernCases.back()}) {
            const bool dead = x >= 121 && x <= 160;
            Check(probe.hold == int(dead) && probe.visualDying == dead,
                  "crocodile back/head boundary or death presentation differs from the ROM");
            Check(probe.drown == int(x >= 145 && x <= 160), "crocodile bite/drown distinction changed");
        }
    }
    Check(BoardVisuals::RiverGatorContact(226, 10) == RiverGatorZone::Back &&
          BoardVisuals::RiverGatorContact(227, 10) == RiverGatorZone::Snout,
          "crocodile contact failed across the 8-bit screen wrap");

    // A single call cannot prove survival: 0x8004 starts a death sequence even
    // when 0x829c is clear. Reach a naturally spawned croc and ride for >96 NMIs.
    for (bool modern : {false, true}) {
        auto game = StartedSim(rom);
        game->Modern(modern);
        game->Poke(0x83b7, 2);
        int guard = 0;
        auto approachSupported = [&]() {
            int x = game->Peek(0x8101) - 50;
            for (int i = 0; i < game->Peek(0x8109); i++) {
                int behind = (game->Peek(0x810a + i) - x) & 255;
                if (behind >= 12 && behind < 43) return true;
            }
            return false;
        };
        while (!(game->Peek(0x8150) & 1) || game->Peek(0x8101) < 140 || game->Peek(0x8101) > 170 || !approachSupported()) {
            game->Step();
            Check(++guard < 2000, "natural river crocodile did not arrive");
        }
        game->Poke(0x8044, game->Peek(0x8101) - 50);
        game->Poke(0x8047, 64);
        game->Poke(0x8004, 0); game->Poke(0x829c, 0); game->Poke(0x83cd, 0);
        for (int frame = 0; frame < 12; frame++) {
            game->Step(1);
            Check(game->Peek(0x8004) == 0, "jump from turtles onto crocodile back killed the frog");
        }
        Check(game->Peek(0x8047) == 48, "upward hop did not land on the crocodile row");
        int startX = game->Peek(0x8044);
        for (int frame = 0; frame < 100; frame++) {
            game->Step();
            Check(game->Peek(0x8004) == 0 && game->Peek(0x81b2) == 0 && game->Peek(0x8047) == 48,
                  "crocodile back started a death or respawn during a sustained ride");
        }
        Check(game->Peek(0x8044) > startX, "safe crocodile did not carry the frog");
        for (int frame = 0; frame < 12; frame++) game->Step(4);
        Check(game->Peek(0x8004) == 0 && game->Peek(0x8044) < startX + 40,
              "frog could not hop on the safe crocodile back");
    }

    auto WriteCrocJson = [](const std::string& path, const std::vector<CrocProbe>& cpList) {
        std::stringstream ss;
        ss << "[\n";
        for (size_t i = 0; i < cpList.size(); i++) {
            ss << "  {\n"
               << "    \"X\": " << cpList[i].x << ",\n"
               << "    \"Hold\": " << cpList[i].hold << ",\n"
               << "    \"Drown\": " << cpList[i].drown << ",\n"
               << "    \"RideTile\": " << cpList[i].rideTile << ",\n"
               << "    \"VisualDying\": " << (cpList[i].visualDying ? "true" : "false") << "\n"
               << "  }" << (i + 1 < cpList.size() ? ",\n" : "\n");
        }
        ss << "]\n";
        WriteFileText(path, ss.str());
    };
    WriteCrocJson("docs/evidence/gator-original-collision.json", cases);
    WriteCrocJson("docs/evidence/gator-modern-collision.json", modernCases);

    // VerifyOriginalLadySpriteClear
    {
        auto game = StartedSim(rom);
        game->Poke(0x8044, 120); game->Poke(0x8047, 224); game->Poke(0x8004, 0); game->Poke(0x83cd, 0);
        game->Poke(0x8134, 0); game->Poke(0x8135, 1); game->Poke(0x813d, 0);
        game->Poke(0x8040, 80); game->Poke(0x8041, 0x21); game->Poke(0x8042, 4); game->Poke(0x8043, 96);
        game->Poke(0x811c, 100); game->Poke(0x833d, 1); game->Poke(0x833e, 50);
        game->Poke(0x8340, 2);

        LadyFrogPresentation lPres;
        lPres.Observe(game->Snapshot(), [&](int a){ return game->Peek(a); });
        for (int frame = 0; frame < 180; frame++) {
            game->Step();
            if (game->Peek(0x8340) == 1 && game->Peek(0x8041) == 0) break;
        }
        lPres.Observe(game->Snapshot(), [&](int a){ return game->Peek(a); });
        float clearVisualX = lPres.X;
        bool clearVisible = lPres.Visible(ModelFootprints::FrogAlongX * 0.62f);
        bool spriteWiped = game->Peek(0x8340) == 1 && game->Peek(0x8135) == 1 && game->Peek(0x8134) == 0 &&
                           game->Peek(0x8040) == 0 && game->Peek(0x8041) == 0 && game->Peek(0x8042) == 0 && game->Peek(0x8043) == 0;
        game->Step();
        lPres.Observe(game->Snapshot(), [&](int a){ return game->Peek(a); });
        float patrolVisualX = lPres.X;
        bool invisiblePatrol = game->Peek(0x8040) >= 8 && game->Peek(0x8040) <= 232 &&
                               game->Peek(0x8041) == 0 && game->Peek(0x8042) == 0 && game->Peek(0x8043) == 0 && game->Peek(0x8135) == 1;
        bool visible = lPres.Visible(ModelFootprints::FrogAlongX * 0.62f);
        int pickupX = game->Peek(0x8040);
        game->Poke(0x8044, pickupX); game->Poke(0x8047, 96);
        game->Poke(0x8004, 0); game->Poke(0x83cd, 0);
        game->Step();

        std::stringstream ladyJson;
        ladyJson << "{\n"
                 << "  \"spriteWiped\": " << (spriteWiped ? "true" : "false") << ",\n"
                 << "  \"clearVisualX\": " << clearVisualX << ",\n"
                 << "  \"clearVisible\": " << (clearVisible ? "true" : "false") << ",\n"
                 << "  \"invisiblePatrol\": " << (invisiblePatrol ? "true" : "false") << ",\n"
                 << "  \"patrolVisualX\": " << patrolVisualX << ",\n"
                 << "  \"presentationVisible\": " << (visible ? "true" : "false") << "\n"
                 << "}\n";
        WriteFileText("docs/evidence/lady-rom-bug.json", ladyJson.str());

        Check(spriteWiped && invisiblePatrol, "original ROM 0x1AA9/0x27DE invisible-lady transition was not reproduced");
        Check(game->Peek(0x8135) != 0 && game->Peek(0x8134) != 0, "original ROM sprite clear no longer allows invisible pickup");
        Check(clearVisible && visible && std::abs(clearVisualX - 80.0f) <= 2.0f && patrolVisualX == pickupX,
              "pink-frog renderer missed or displaced her after ROM 0x27DE blanked the shared sprite");
    }

    for (int side : {-1, 1}) {
        auto impact = StartedSim(rom);
        impact->Modern(true);
        for (int lane = 6; lane <= 10; lane++) impact->Poke(0x8100 + lane * 9, 0);
        impact->Poke(0x8148, 1);
        impact->Poke(0x8149, 132);
        impact->Poke(0x8044, 120 + side * 12);
        impact->Poke(0x8047, 176);
        impact->Poke(0x8004, 0);
        impact->Poke(0x83cd, 0);
        for (int a = 0x8248; a <= 0x8253; a++) impact->Poke(a, 0);
        impact->Step();
        Check(impact->Peek(0x8004) != 0, "model collision failed on side of vehicle");
    }

    std::stringstream fbJson;
    fbJson << "{\n"
           << "  \"wrapping\": true,\n"
           << "  \"deaths\": [\n"
           << "    {\"water\": false, \"frames\": " << deaths[0].frames << ", \"subcounterResets\": " << deaths[0].subcounterResets << ", \"monotonic\": true},\n"
           << "    {\"water\": true, \"frames\": " << deaths[1].frames << ", \"subcounterResets\": " << deaths[1].subcounterResets << ", \"monotonic\": true}\n"
           << "  ],\n"
           << "  \"bonuses\": [\n";
    for (size_t i = 0; i < bonuses.size(); i++) {
        fbJson << "    {\"bug\": " << (bonuses[i].bug ? "true" : "false") << ", \"rider\": " << (bonuses[i].rider ? "true" : "false")
               << ", \"fifth\": " << (bonuses[i].fifth ? "true" : "false") << ", \"events\": " << bonuses[i].events << "}"
               << (i + 1 < bonuses.size() ? ",\n" : "\n");
    }
    fbJson << "  ],\n"
           << "  \"turtleMaximumStep\": " << maxStep << ",\n"
           << "  \"turtleJumps\": [\n" << turtleJumpsJson.str() << "\n  ],\n"
           << "  \"heldInputFrames\": " << heldInputFrames << ",\n"
           << "  \"singlePressX\": " << singlePressX << ",\n"
           << "  \"repressedX\": " << held->Peek(0x8044) << ",\n"
           << "  \"frogNativeStalls\": " << frogNativeStalls << ",\n"
           << "  \"frogVisualMovesDuringStalls\": " << frogVisualMovesDuringStalls << ",\n"
           << "  \"maxFrogLag\": " << maxFrogLag << ",\n"
           << "  \"lowerGrassRow\": " << bottom->Peek(0x8047) << ",\n"
           << "  \"nativeObjectMoves\": " << nativeMoves << ",\n"
           << "  \"nativeObjectStalls\": " << nativeStalls << ",\n"
           << "  \"subpixelMovesDuringNativeStalls\": " << smoothedDuringStall << ",\n"
           << "  \"maxVisualLag\": " << maxVisualLag << ",\n"
           << "  \"exportedModelBoundsVerified\": 6,\n"
           << "  \"vehicleFrontAndBackEdgesVerified\": 5,\n"
           << "  \"modelCollisionLiveSides\": 2\n"
           << "}\n";
    WriteFileText("docs/evidence/feedback-tests.json", fbJson.str());
    std::cout << "Feedback regressions: wrapping, death/bonus continuity, smooth diving, single-press hops, both grass rows and model-bound road collision passed" << std::endl;
}

// ---------------- Main Test Runner ---------------- //

int main() {
    try {
        auto rom = ReadFileBytes("godot/rom/maincpu.bin");
        ArcadeSimulationCore game(rom);
        auto tStart = std::chrono::high_resolution_clock::now();
        bool play = false, hop = false;
        std::ofstream trace("docs/evidence/native-frames.bin", std::ios::binary);
        if (!trace.is_open()) throw std::runtime_error("Could not create native-frames.bin");

        for (int frame = 0; frame < 650; frame++) {
            int input = (frame >= 150 && frame < 156) ? 16 :
                        (frame >= 230 && frame < 236) ? 32 :
                        (frame >= 340 && frame < 346) ? 1 : 0;
            game.Step(input);
            const auto& state = game.StateArray();
            trace.write(reinterpret_cast<const char*>(state.data()), state.size());
            play |= (game.Peek(0x83fe) != 0);
            hop |= (game.Peek(0x8047) == 0xd0 && game.Peek(0x83fe) != 0);
        }
        trace.close();

        auto tEnd = std::chrono::high_resolution_clock::now();
        double ms = std::chrono::duration<double, std::milli>(tEnd - tStart).count();
        if (!play || !hop) throw std::runtime_error("Native C++ runtime did not start and hop");

        // Write csharp-state.json and csharp-host.json (same format as original for compatibility)
        {
            std::stringstream ss;
            ss << "[";
            const auto& st = game.StateArray();
            for (size_t i = 0; i < st.size(); i++) {
                ss << (int)st[i] << (i + 1 < st.size() ? "," : "");
            }
            ss << "]";
            WriteFileText("docs/evidence/csharp-state.json", ss.str());
        }
        {
            std::stringstream ss;
            ss << "{\"frames\":650,\"play\":" << (play ? "true" : "false") << ",\"hop\":" << (hop ? "true" : "false")
               << ",\"milliseconds\":" << ms << ",\"millisecondsPerFrame\":" << (ms / 650.0) << "}";
            WriteFileText("docs/evidence/csharp-host.json", ss.str());
        }
        std::cout << "Native host: 650 frames, play=" << (play ? "true" : "false") << ", hop=" << (hop ? "true" : "false")
                  << ", " << std::fixed << std::setprecision(3) << (ms / 650.0) << " ms/frame" << std::endl;

        // Independent oracle: MAME fixtures
        struct MameFixtureResult {
            std::string address;
            int instructions;
            int bytes;
            int diff;
            int registerDifferences;
        };
        std::vector<MameFixtureResult> fixtures;
        for (const auto& entry : fs::directory_iterator("docs/evidence/mame-functions")) {
            std::string path = entry.path().string();
            if (path.size() >= 11 && path.substr(path.size() - 11) == "-before.txt") {
                std::string stem = path.substr(0, path.size() - 11);
                std::string beforeText = ReadFileText(path);
                std::string afterText = ReadFileText(stem + "-after.txt");
                std::vector<uint8_t> memory = ReadFileBytes(stem + "-before.bin");
                std::vector<uint8_t> expected = ReadFileBytes(stem + "-after.bin");

                std::vector<int> beforeRegs, afterRegs;
                {
                    std::stringstream ss(beforeText);
                    int val;
                    while (ss >> val) beforeRegs.push_back(val);
                }
                {
                    std::stringstream ss(afterText);
                    int val;
                    while (ss >> val) afterRegs.push_back(val);
                }

                ArcadeSimulationCore native(rom);
                std::copy(memory.begin(), memory.begin() + 2048, native.Bus.Ram);
                std::copy(memory.begin() + 2048, memory.begin() + 3072, native.Bus.Video);
                std::copy(memory.begin() + 3072, memory.begin() + 3328, native.Bus.Objects);

                auto& cpu = native.Cpu;
                cpu.PC = (uint16_t)beforeRegs[0];
                cpu.AF = (uint16_t)beforeRegs[3];
                cpu.BC = (uint16_t)beforeRegs[4];
                cpu.DE = (uint16_t)beforeRegs[5];
                cpu.HL = (uint16_t)beforeRegs[6];
                cpu.IX = (uint16_t)beforeRegs[7];
                cpu.IY = (uint16_t)beforeRegs[8];
                cpu.SP = (uint16_t)beforeRegs[9];

                int instructions = 0;
                do {
                    cpu.StepInstruction();
                    if (++instructions > 1000000) throw std::runtime_error("MAME fixture did not return: " + path);
                } while (cpu.PC != (uint16_t)beforeRegs[1] || cpu.SP != (uint16_t)(beforeRegs[2] + 2));

                const auto& actual = native.StateArray();
                int diff = 0;
                for (size_t i = 0; i < expected.size() && i < actual.size(); i++) {
                    if (actual[i] != expected[i]) diff++;
                }

                int regs[] = {cpu.AF, cpu.BC, cpu.DE, cpu.HL, cpu.IX, cpu.IY, cpu.SP};
                int regDiff = 0;
                for (size_t i = 0; i < 7; i++) {
                    if (regs[i] != afterRegs[i + 3]) regDiff++;
                }

                std::stringstream hexAddr;
                hexAddr << std::hex << std::setw(4) << std::setfill('0') << beforeRegs[0];
                fixtures.push_back({hexAddr.str(), instructions, 3328, diff, regDiff});
                if (diff != 0 || regDiff != 0) {
                    throw std::runtime_error("MAME fixture mismatch at " + hexAddr.str());
                }
            }
        }
        if (fixtures.size() != 8) throw std::runtime_error("Expected eight independent MAME fixtures");
        std::sort(fixtures.begin(), fixtures.end(), [](const auto& a, const auto& b){ return a.address < b.address; });

        {
            std::stringstream ss;
            ss << "[\n";
            for (size_t i = 0; i < fixtures.size(); i++) {
                ss << "  {\n"
                   << "    \"address\": \"" << fixtures[i].address << "\",\n"
                   << "    \"instructions\": " << fixtures[i].instructions << ",\n"
                   << "    \"bytes\": " << fixtures[i].bytes << ",\n"
                   << "    \"diff\": " << fixtures[i].diff << ",\n"
                   << "    \"registerDifferences\": " << fixtures[i].registerDifferences << "\n"
                   << "  }" << (i + 1 < fixtures.size() ? ",\n" : "\n");
            }
            ss << "]\n";
            WriteFileText("docs/evidence/native-vs-mame.json", ss.str());
        }
        std::cout << "Eight MAME function fixtures: exact RAM and register match" << std::endl;

        // Native Sound
        auto audioRom = ReadFileBytes("godot/rom/audiocpu.bin");
        ArcadeSimulationCore audioGame(rom, false, audioRom);
        double audioPeak = 0;
        long long sampleCount = 0;
        for (int frame = 0; frame < 1800; frame++) {
            int input = (frame >= 150 && frame < 156) ? 16 :
                        (frame >= 230 && frame < 236) ? 32 :
                        (frame > 340 && (frame % 30) < 6) ? 1 : 0;
            audioGame.Step(input);
            while (!audioGame.Sound->Samples.empty()) {
                float sample = audioGame.Sound->Samples.front();
                audioGame.Sound->Samples.pop_front();
                audioPeak = std::max(audioPeak, (double)std::abs(sample));
                sampleCount++;
            }
        }
        if (audioPeak < 0.01) throw std::runtime_error("Native sound ROM never produced audible output");

        int executedCount = 0;
        for (bool b : audioGame.Sound->Cpu.Executed) if (b) executedCount++;
        {
            std::stringstream ss;
            ss << "{\"frames\":1800,\"samples\":" << sampleCount << ",\"audioPeak\":" << audioPeak
               << ",\"executedAddresses\":" << executedCount << "}";
            WriteFileText("docs/evidence/native-sound.json", ss.str());
        }
        std::cout << "Native sound: " << sampleCount << " samples, peak " << std::fixed << std::setprecision(3)
                  << audioPeak << ", " << executedCount << " executed addresses" << std::endl;

        auto soundProbe = audioGame.Sound.get();
        for (int cmd : {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,24,48,128,144,176,208,240,255}) {
            soundProbe->Command(cmd);
            soundProbe->AdvanceTo(soundProbe->Cpu.Cycles + NativeSound::Clock * 2);
            soundProbe->Samples.clear();
        }
        {
            std::stringstream ss;
            ss << "{\"executed\":[";
            bool first = true;
            for (int a = 0; a < 65536; a++) {
                if (soundProbe->Cpu.Executed[a]) {
                    if (!first) ss << ",";
                    ss << a;
                    first = false;
                }
            }
            ss << "],\"indirectTargets\":[";
            std::vector<int> indirect(soundProbe->Cpu.IndirectTargets.begin(), soundProbe->Cpu.IndirectTargets.end());
            std::sort(indirect.begin(), indirect.end());
            for (size_t i = 0; i < indirect.size(); i++) {
                ss << indirect[i] << (i + 1 < indirect.size() ? "," : "");
            }
            ss << "]}";
            WriteFileText("docs/evidence/sound-execution.json", ss.str());
        }

        if (!ArcadeSimulationCore::SweptBox(0, 0, 100, 0, 50, 0, 3, 3) ||
            ArcadeSimulationCore::SweptBox(0, 7, 100, 7, 50, 0, 3, 3)) {
            throw std::runtime_error("Swept collision tunneling/gap regression");
        }

        // Lifecycle tests
        ArcadeSimulationCore idle(rom);
        int timerDeaths = 0, lastHold = 0;
        bool everPlaying = false, gameOver = false;
        for (int frame = 0; frame < 9000; frame++) {
            idle.Step((frame >= 150 && frame < 156) ? 16 : (frame >= 230 && frame < 236) ? 32 : 0);
            int hold = idle.Peek(0x8004);
            if (idle.Peek(0x83fe) != 0 && hold != 0 && lastHold == 0) timerDeaths++;
            lastHold = hold;
            everPlaying |= (idle.Peek(0x83fe) != 0);
            if (everPlaying && idle.Peek(0x83fe) == 0) gameOver = true;
        }
        if (timerDeaths < 3 || !gameOver) throw std::runtime_error("Timer/lives/game-over path failed");

        ArcadeSimulationCore two(rom);
        bool playerTwo = false;
        for (int frame = 0; frame < 900; frame++) {
            int input = ((frame >= 150 && frame < 156) || (frame >= 170 && frame < 176)) ? 16 :
                        (frame >= 230 && frame < 236) ? 64 : 0;
            if (frame == 380) { two.Poke(0x8004, 1); two.Poke(0x83cd, 0); }
            two.Step(input);
            playerTwo |= (two.Peek(0x83fe) == 2 && two.Peek(0x83fd) == 2);
        }
        if (!playerTwo) throw std::runtime_error("Two-player life hand-off never reached player two");

        ArcadeSimulationCore goals(rom);
        for (int frame = 0; frame < 310; frame++) {
            goals.Step((frame >= 150 && frame < 156) ? 16 : (frame >= 230 && frame < 236) ? 32 : 0);
        }
        for (int bay = 0; bay < 5; bay++) {
            goals.Poke(0x8044, 24 + 48 * bay);
            goals.Poke(0x8047, 32);
            goals.Poke(0x8004, 0);
            goals.Poke(0x83cd, 0);
            goals.Poke(0x8122, 1);
            for (int f = 0; f < 180; f++) goals.Step();
        }
        for (int f = 0; f < 500; f++) goals.Step();
        bool nextLevel = (goals.Peek(0x83b7) >= 2);
        if (!nextLevel) throw std::runtime_error("Five home goals failed to advance the board");

        {
            std::stringstream ss;
            ss << "{\"idleFrames\":9000,\"timerDeaths\":" << timerDeaths << ",\"gameOver\":" << (gameOver ? "true" : "false")
               << ",\"playerTwo\":" << (playerTwo ? "true" : "false") << ",\"nextLevel\":" << (nextLevel ? "true" : "false")
               << ",\"score\":" << goals.Snapshot().BcdScore(0x83ed) << "}";
            WriteFileText("docs/evidence/lifecycle-tests.json", ss.str());
        }
        std::cout << "Lifecycle: " << timerDeaths << " timer deaths, game-over, two-player hand-off, all homes -> level "
                  << goals.Peek(0x83b7) << std::endl;

        // Presentation turtle phases
        ArcadeSimulationCore dive(rom);
        std::set<int> divePhases;
        for (int frame = 0; frame < 1800; frame++) {
            dive.Step((frame >= 150 && frame < 156) ? 16 : (frame >= 230 && frame < 236) ? 32 : 0);
            if (frame < 300) continue;
            auto s = dive.Snapshot();
            for (int lane : {1, 4}) {
                int count = std::min(8, s.At(0x8100 + 9 * lane));
                for (int idx = 0; idx < count; idx++) {
                    float center = s.At(0x8101 + 9 * lane + idx) - 12 - (lane == 1 ? 31 : 47) / 2.0f;
                    divePhases.insert(BoardVisuals::TurtlePhase(s, center, (lane + 3) * 16));
                }
            }
        }
        if (divePhases.find(0) == divePhases.end() || divePhases.find(2) == divePhases.end()) {
            throw std::runtime_error("Renderer did not see both surfaced and submerged original turtle tiles");
        }
        {
            std::stringstream ss;
            ss << "{\"turtlePhases\":[";
            bool first = true;
            for (int p : divePhases) {
                if (!first) ss << ",";
                ss << p;
                first = false;
            }
            ss << "],\"source\":\"original VRAM and object scroll registers\"}";
            WriteFileText("docs/evidence/presentation-tests.json", ss.str());
        }
        std::cout << "Turtle presentation follows original surface, warning and submerged tiles" << std::endl;

        RunFeedbackTests(rom);
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "FATAL ERROR: " << ex.what() << std::endl;
        return 1;
    }
}
