namespace FroggerRemake;

public readonly record struct GameOptions(bool ModernCollision,bool Perspective,bool Follow,bool Fullscreen);

public static class GameDefaults
{
    public const int PreferencesVersion=2;
    public static GameOptions FromStored(int version,bool modern,bool perspective,bool follow,bool fullscreen)=>
        version>=PreferencesVersion?new(modern,perspective,follow,fullscreen):new(true,true,true,true);
}
