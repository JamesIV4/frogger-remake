using FroggerRemake;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.Json;

internal static class FeedbackTests
{
    public static ArcadeSimulation Started(byte[] rom) {
        var s=new ArcadeSimulation(rom);
        for(int f=0;f<310;f++)s.Step(f>=150&&f<156?16:f>=230&&f<236?32:0);
        return s;
    }
    public static void Run(byte[] rom) {
        // Both sides of a long log stay visible until the actual geometry exits.
        Check(BoardVisuals.IntersectsPlayfield(-25,46),"left long log culled too early");
        Check(BoardVisuals.IntersectsPlayfield(260,46),"right long log culled too early");
        Check(!BoardVisuals.IntersectsPlayfield(-40,46),"fully offscreen log was retained");
        Check(Math.Abs(BoardVisuals.InterpolateByte(255,0,.5f)-255.5f)<.001f,"byte wrap interpolates through the board");
        Check(Math.Abs(BoardVisuals.InterpolateByte(0,255,.5f)+.5f)<.001f,"reverse wrap interpolates through the board");
        var deaths=new List<object>();
        foreach(bool water in new[]{false,true}) {
            using var sim=Started(rom);var view=new FrogVisualState();
            sim.Poke(0x8044,120);sim.Poke(0x8047,water?112:208);sim.Poke(0x8004,1);sim.Poke(0x829c,water?1:0);sim.Poke(0x83cd,0);
            double previousAge=-1;float previousDepth=-1,previousCompression=-1;int resets=0,lastCounter=-1,frames=0,anchorX=0,anchorRow=0;
            for(int n=0;n<100;n++) {
                sim.Step();var state=sim.Snapshot();view.Observe(state);
                if(!view.Dying){if(frames>0)break;continue;}
                double age=view.DeathSeconds(state.frame,0);
                Check(age>previousAge,"death clock restarted at a ROM subphase");
                Check(view.Drowning==water,"death cause changed mid-animation");
                float depth=FrogVisualState.DrownDepth(age),compression=FrogVisualState.SquashProgress(age);
                Check(depth>=previousDepth&&compression>=previousCompression,"death geometry rebounded upward");
                previousDepth=depth;previousCompression=compression;
                if(frames==0){anchorX=state.At(0x8044);anchorRow=state.At(0x8047);}
                Check(view.DeathX==anchorX&&view.DeathRow==anchorRow,"death anchor drifted");
                if(state.At(0x8247)<lastCounter)resets++;
                lastCounter=state.At(0x8247);previousAge=age;frames++;
            }
            Check(resets>=3,"test never crossed the resetting ROM counter");
            deaths.Add(new{water,frames,subcounterResets=resets,monotonic=true});
        }
        var bonuses=new List<object>();
        foreach(var (bug,rider,fifth) in new[]{(false,false,false),(true,false,false),(false,true,false),(true,true,false),(true,true,true)}) {
            using var sim=Started(rom);int bay=fifth?5:1;
            if(fifth){for(int i=0;i<4;i++)sim.Poke(0x825e+i,1);sim.Poke(0x825c,4);}
            sim.Poke(0x8044,24+48*(bay-1));sim.Poke(0x8047,32);sim.Poke(0x8004,0);sim.Poke(0x83cd,0);
            sim.Poke(0x8120,0);sim.Poke(0x8121,bug?bay:0);sim.Poke(0x8122,1);
            sim.Poke(0x8134,rider?1:0);sim.Poke(0x8135,rider?1:0);
            var carried=new FrogVisualState();carried.Observe(sim.Snapshot());
            Check(carried.Carrying==rider,"carried frog flag was not exposed");
            int before=sim.Snapshot().BcdScore(0x83ed);
            for(int f=0;f<10;f++)sim.Step();
            var awards=sim.BonusAwards.ToArray();int expected=(bug?1:0)+(rider?1:0)+1;
            Check(awards.Length==expected,$"bonus call count: {bug}/{rider}/{fifth} got {awards.Length}");
            Check(awards.Count(a=>a.Kind==BonusKind.Bug)==(bug?1:0),"bug bonus missing/duplicated");
            Check(awards.Count(a=>a.Kind==BonusKind.Rescue)==(rider?1:0),"rescue bonus missing/duplicated");
            Check(awards.Count(a=>a.Kind==BonusKind.Time)==1,"time bonus missing/duplicated");
            Check(awards.All(a=>(a.Kind==BonusKind.Time||a.Amount==200)&&a.X==24+48*(bay-1)),"bonus amount or origin changed: "+JsonSerializer.Serialize(awards));
            Check(sim.Snapshot().BcdScore(0x83ed)-before>=awards.Sum(a=>a.Amount),"popup had no matching score award");
            bonuses.Add(new{bug,rider,fifth,events=awards.Length});
        }
        using var turtles=Started(rom);
        var prior=new Dictionary<(int,int),(float depth,int phase,int clock)>();
        float maxStep=0;var jumps=new List<object>();
        for(int f=0;f<1200;f++) {
            turtles.Step();var s=turtles.Snapshot();
            foreach(int lane in new[]{1,4})for(int index=0;index<Math.Min(8,s.At(0x8100+lane*9));index++) {
                float x=s.At(0x8101+lane*9+index)-12-(lane==1?31:47)/2f;
                int row=(lane+3)*16,phase=BoardVisuals.TurtlePhase(s,x,row),clock=s.At(lane==1?0x8110:0x8111);
                float depth=BoardVisuals.TurtleDepth(s,x,row);
                if(prior.TryGetValue((lane,index),out var old)) {
                    float delta=Math.Abs(depth-old.depth);maxStep=Math.Max(maxStep,delta);
                    if(delta>.13f&&jumps.Count<8)jumps.Add(new{s.frame,lane,index,old.depth,newDepth=depth,oldPhase=old.phase,phase,oldClock=old.clock,clock});
                }
                prior[(lane,index)]=(depth,phase,clock);
            }
        }
        File.WriteAllText("docs/evidence/feedback-tests.json",JsonSerializer.Serialize(new{wrapping=true,deaths,bonuses,turtleMaximumStep=maxStep,turtleJumps=jumps},new JsonSerializerOptions{WriteIndented=true}));
        Check(maxStep<.13f,$"turtle dive jumps {maxStep}; see feedback-tests.json");
        Console.WriteLine("Feedback regressions: wrapping, death continuity, carried frog, both +200 awards, fifth-home awards and smooth diving passed");
    }
    private static void Check(bool condition,string message){if(!condition)throw new Exception(message);}
}
