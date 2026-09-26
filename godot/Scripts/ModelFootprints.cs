// Generated from the Blender mesh bounds in art/scripts/build_assets.py.
namespace FroggerRemake;
public readonly record struct ModelFootprint(float MinAlongX,float MaxAlongX,float AcrossRow);
public static class ModelFootprints {
    public const float FrogAlongX=8.56170f;
    public const float FrogAcrossRow=6.89952f;
    public const float RiverGatorLengthTiles=2.153474f;
    public const float RiverGatorWidthTiles=1.100000f;
    public const float RiverGatorFrontTiles=1.030000f;
    public const float RiverGatorSnoutTiles=0.605000f;
    public const float LogTopTiles=0.436645f;
    public const float SnakeBottomTiles=0.021363f;
    public const float LadyBottomTiles=-0.006061f;
    // Vehicles rotate their local length axis into the ROM X direction.
    // Presentation scales every vehicle root to 0.84.
    public static ModelFootprint Vehicle(int lane)=>lane switch {
        6=>new(-13.09056f,13.30560f,6.07488f), // truck
        7=>new(-7.79520f,8.29920f,6.07488f), // sport
        8=>new(-8.29920f,7.79520f,6.07488f), // car
        9=>new(-6.76704f,8.66880f,6.24960f), // dozer
        10=>new(-8.87040f,6.76704f,6.07488f), // racecar
        _=>throw new System.ArgumentOutOfRangeException(nameof(lane))
    };
}
