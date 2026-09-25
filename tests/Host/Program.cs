using System;
using System.Diagnostics;
using System.IO;
using FroggerRemake;
using System.Text.Json;
using System.Linq;
using System.Collections.Generic;

var rom = File.ReadAllBytes("godot/rom/maincpu.bin");
using var game = new ArcadeSimulation(rom);
var timer=Stopwatch.StartNew();
bool play=false, hop=false;
using var trace=File.Create("docs/evidence/native-frames.bin");
for (int frame=0;frame<650;frame++) {
    int input=frame>=150 && frame<156 ? 16 : frame>=230 && frame<236 ? 32 : frame>=340 && frame<346 ? 1 : 0;
    game.Step(input);
    trace.Write(game.StateArray());
    play |= game.Peek(0x83fe)!=0;
    hop |= game.Peek(0x8047)==0xd0 && game.Peek(0x83fe)!=0;
}
var ms=timer.Elapsed.TotalMilliseconds;
if(!play || !hop) throw new Exception("Godot's C# runtime did not start and hop");
File.WriteAllText("docs/evidence/csharp-state.json",game.StateBytes());
File.WriteAllText("docs/evidence/csharp-host.json",JsonSerializer.Serialize(new {frames=650,play,hop,milliseconds=ms, millisecondsPerFrame=ms/650}));
Console.WriteLine($"C# host: 650 frames, play={play}, hop={hop}, {ms/650:F3} ms/frame");

// Independent oracle: MAME captured both sides of actual function invocations.
var fixtures=new List<object>();
foreach(string entry in Directory.GetFiles("docs/evidence/mame-functions","*-before.txt")) {
    string stem=entry[..^11];
    int[] before=File.ReadAllText(entry).Split((char[]?)null,StringSplitOptions.RemoveEmptyEntries).Select(int.Parse).ToArray();
    int[] after=File.ReadAllText(stem+"-after.txt").Split((char[]?)null,StringSplitOptions.RemoveEmptyEntries).Select(int.Parse).ToArray();
    byte[] memory=File.ReadAllBytes(stem+"-before.bin"),expected=File.ReadAllBytes(stem+"-after.bin");
    using var native=new ArcadeSimulation(rom);
    Array.Copy(memory,0,native.Bus.Ram,0,2048);Array.Copy(memory,2048,native.Bus.Video,0,1024);Array.Copy(memory,3072,native.Bus.Objects,0,256);
    var cpu=native.Cpu;cpu.PC=before[0];cpu.AF=before[3];cpu.BC=before[4];cpu.DE=before[5];cpu.HL=before[6];cpu.IX=before[7];cpu.IY=before[8];cpu.SP=before[9];
    int instructions=0;
    do {cpu.StepInstruction();if(++instructions>1000000)throw new Exception("MAME fixture did not return: "+entry);}while(cpu.PC!=before[1]||cpu.SP!=before[2]+2);
    var actual=native.StateArray();int diff=actual.Zip(expected).Count(pair=>pair.First!=pair.Second);
    int[] regs={cpu.AF,cpu.BC,cpu.DE,cpu.HL,cpu.IX,cpu.IY,cpu.SP};
    int regDiff=regs.Zip(after.Skip(3)).Count(pair=>pair.First!=pair.Second);
    fixtures.Add(new {address=before[0].ToString("x4"),instructions,bytes=3328,diff,registerDifferences=regDiff});
    if(diff!=0||regDiff!=0)throw new Exception($"MAME fixture mismatch {before[0]:x4}: {diff} bytes, {regDiff} regs");
}
if(fixtures.Count!=5)throw new Exception("Expected five independent MAME fixtures");
File.WriteAllText("docs/evidence/native-vs-mame.json",JsonSerializer.Serialize(fixtures,new JsonSerializerOptions{WriteIndented=true}));
Console.WriteLine("Five MAME function fixtures: exact RAM and register match");

using var audioGame=new ArcadeSimulation(rom,false,File.ReadAllBytes("godot/rom/audiocpu.bin"));
double audioPeak=0;long samples=0;
for(int frame=0;frame<1800;frame++){
    int input=frame>=150&&frame<156?16:frame>=230&&frame<236?32:frame>340&&frame%30<6?1:0;
    audioGame.Step(input);
    while(audioGame.Sound!.Samples.TryDequeue(out float sample)){audioPeak=Math.Max(audioPeak,Math.Abs(sample));samples++;}
}
if(audioPeak<.01)throw new Exception("Native sound ROM never produced audible output");
File.WriteAllText("docs/evidence/native-sound.json",JsonSerializer.Serialize(new{frames=1800,samples,audioPeak,executedAddresses=audioGame.Sound!.Cpu.Executed.Count(x=>x)}));
Console.WriteLine($"Native sound: {samples} samples, peak {audioPeak:F3}, {audioGame.Sound.Cpu.Executed.Count(x=>x)} executed addresses");
var soundProbe=audioGame.Sound;
foreach(int command in new[]{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,24,48,128,144,176,208,240,255}){
    soundProbe.Command(command);soundProbe.AdvanceTo(soundProbe.Cpu.Cycles+NativeSound.Clock*2);soundProbe.Samples.Clear();
}
File.WriteAllText("docs/evidence/sound-execution.json",JsonSerializer.Serialize(new{executed=Enumerable.Range(0,65536).Where(a=>soundProbe.Cpu.Executed[a]).ToArray(),indirectTargets=soundProbe.Cpu.IndirectTargets.Order().ToArray()}));
if(!ArcadeSimulation.SweptBox(0,0,100,0,50,0,3,3)||ArcadeSimulation.SweptBox(0,7,100,7,50,0,3,3))throw new Exception("Swept collision tunneling/gap regression");

using var idle=new ArcadeSimulation(rom);
int timerDeaths=0,lastHold=0;bool everPlaying=false,gameOver=false;
for(int frame=0;frame<9000;frame++){
    idle.Step(frame>=150&&frame<156?16:frame>=230&&frame<236?32:0);
    int hold=idle.Peek(0x8004);
    if(idle.Peek(0x83fe)!=0&&hold!=0&&lastHold==0)timerDeaths++;
    lastHold=hold;everPlaying|=idle.Peek(0x83fe)!=0;
    if(everPlaying&&idle.Peek(0x83fe)==0)gameOver=true;
}
if(timerDeaths<3||!gameOver)throw new Exception($"Timer/lives/game-over path failed: deaths={timerDeaths} gameOver={gameOver}");
using var two=new ArcadeSimulation(rom);
bool playerTwo=false;
for(int frame=0;frame<900;frame++){
    int input=(frame>=150&&frame<156)||(frame>=170&&frame<176)?16:frame>=230&&frame<236?64:0;
    if(frame==380){two.Poke(0x8004,1);two.Poke(0x83cd,0);}
    two.Step(input);playerTwo|=two.Peek(0x83fe)==2&&two.Peek(0x83fd)==2;
}
if(!playerTwo)throw new Exception("Two-player life hand-off never reached player two");
using var goals=new ArcadeSimulation(rom);
for(int frame=0;frame<310;frame++)goals.Step(frame>=150&&frame<156?16:frame>=230&&frame<236?32:0);
for(int bay=0;bay<5;bay++){
    goals.Poke(0x8044,24+48*bay);goals.Poke(0x8047,32);goals.Poke(0x8004,0);goals.Poke(0x83cd,0);goals.Poke(0x8122,1);
    for(int frame=0;frame<180;frame++)goals.Step();
}
for(int frame=0;frame<500;frame++)goals.Step();
bool nextLevel=goals.Peek(0x83b7)>=2;
if(!nextLevel)throw new Exception("Five home goals failed to advance the board");
File.WriteAllText("docs/evidence/lifecycle-tests.json",JsonSerializer.Serialize(new{idleFrames=9000,timerDeaths,gameOver,playerTwo,nextLevel,score=goals.Snapshot().BcdScore(0x83ed)}));
Console.WriteLine($"Lifecycle: {timerDeaths} timer deaths, game-over, two-player hand-off, all homes -> level {goals.Peek(0x83b7)}");
using var dive=new ArcadeSimulation(rom);
var divePhases=new HashSet<int>();
for(int frame=0;frame<1800;frame++){
    dive.Step(frame>=150&&frame<156?16:frame>=230&&frame<236?32:0);
    if(frame<300)continue;
    var s=dive.Snapshot();
    foreach(int lane in new[]{1,4})for(int index=0;index<Math.Min(8,s.At(0x8100+9*lane));index++){
        float center=s.At(0x8101+9*lane+index)-12-(lane==1?31:47)/2f;
        divePhases.Add(BoardVisuals.TurtlePhase(s,center,(lane+3)*16));
    }
}
if(!divePhases.Contains(0)||!divePhases.Contains(2))throw new Exception("Renderer did not see both surfaced and submerged original turtle tiles: "+string.Join(",",divePhases));
File.WriteAllText("docs/evidence/presentation-tests.json",JsonSerializer.Serialize(new{turtlePhases=divePhases.Order().ToArray(),source="original VRAM and object scroll registers"}));
Console.WriteLine("Turtle presentation follows original surface, warning and submerged tiles");
