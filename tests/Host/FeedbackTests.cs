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
        var motion=new PresentationMotion();
        Check(motion.Step(1,0,1f/60)==0,"moving model did not initialize at native position");
        float moving=motion.Step(1,1,1f/60),settling=motion.Step(1,1,1f/60);
        Check(moving>0&&moving<1&&settling>moving&&settling<1,"visual step was not subpixel or did not settle");
        motion.Step(2,255,1f/60);
        Check(motion.Step(2,0,1f/60)>255,"moving model teleported across the ROM byte wrap");
        motion.Reset();Check(motion.Step(1,80,1f/60)==80,"motion reset retained a prior level position");
        using var movingBoard=Started(rom);var laneMotion=new PresentationMotion();
        int nativeStalls=0,smoothedDuringStall=0,nativeMoves=0;float maxVisualLag=0;
        var lastNative=new Dictionary<int,int>();var lastShown=new Dictionary<int,float>();
        for(int f=0;f<180;f++){
            movingBoard.Step();
            foreach(int lane in new[]{0,1,2,3,4,6,7,8,9,10}){
                int table=0x8100+lane*9;if(movingBoard.Peek(table)==0)continue;
                int raw=movingBoard.Peek(table+1);
                laneMotion.Step(lane,raw,1f/120);float shown=laneMotion.Step(lane,raw,1f/120);
                float lag=raw-shown;lag-=256f*MathF.Round(lag/256f);maxVisualLag=Math.Max(maxVisualLag,Math.Abs(lag));
                if(lastNative.TryGetValue(lane,out int old)){
                    if(old==raw){nativeStalls++;if(Math.Abs(shown-lastShown[lane])>.001f)smoothedDuringStall++;}
                    else nativeMoves++;
                }
                lastNative[lane]=raw;lastShown[lane]=shown;
            }
        }
        Check(nativeMoves>0&&nativeStalls>0&&smoothedDuringStall>0,"actual ROM object motion did not receive subpixel presentation between native steps");
        Check(maxVisualLag<2.5f,"smoothing placed moving models too far from their native collision positions");
        var deaths=new List<object>();
        Check(FrogVisualState.DrownDepth(.1)>.3f,"water death waits visibly above the surface");
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
        float maxStep=0;var jumps=new List<object>();var knownDiving=new HashSet<(int,int)>();
        for(int f=0;f<1200;f++) {
            turtles.Step();var s=turtles.Snapshot();
            foreach(int lane in new[]{1,4})for(int index=0;index<Math.Min(8,s.At(0x8100+lane*9));index++) {
                float x=s.At(0x8101+lane*9+index)-12-(lane==1?31:47)/2f;
                int row=(lane+3)*16,phase=BoardVisuals.TurtlePhase(s,x,row),clock=s.At(lane==1?0x8110:0x8111);
                if(phase>0)knownDiving.Add((lane,index));
                float depth=BoardVisuals.TurtleDepth(s,x,row,0,knownDiving.Contains((lane,index)));
                if(prior.TryGetValue((lane,index),out var old)) {
                    float delta=Math.Abs(depth-old.depth);maxStep=Math.Max(maxStep,delta);
                    if(delta>.13f&&jumps.Count<8)jumps.Add(new{s.frame,lane,index,old.depth,newDepth=depth,oldPhase=old.phase,phase,oldClock=old.clock,clock});
                }
                prior[(lane,index)]=(depth,phase,clock);
            }
        }
        Check(maxStep<.13f,$"turtle dive jumps {maxStep}; see feedback-tests.json");
        using var right=Started(rom);
        var hopTrace=new List<object>();
        for(int f=0;f<96;f++){
            right.Step(f<80?8:0);var s=right.Snapshot();
            hopTrace.Add(new{frame=f,x=s.At(0x8044),row=s.At(0x8047),flags=Enumerable.Range(0,4).Select(i=>s.At(0x8248+i)).ToArray(),counters=Enumerable.Range(0,4).Select(i=>s.At(0x8250+i)).ToArray()});
        }
        File.WriteAllText("docs/evidence/hop-trace.json",JsonSerializer.Serialize(hopTrace,new JsonSerializerOptions{WriteIndented=true}));
        using var held=Started(rom);
        var hopView=new FrogVisualState();int lastActive=-1,landedFrames=0;
        for(int f=0;f<96;f++){
            int button=InputPulse.ForHeldDirection(8,held.Snapshot());held.Step(button);var s=held.Snapshot();hopView.Observe(s);
            bool nativeHop=Enumerable.Range(0,4).Any(i=>s.At(0x8248+i)!=0);
            if(nativeHop)lastActive=f;
            if(!nativeHop&&hopView.HopActive(s.frame)&&lastActive>=0&&f-lastActive<=2)landedFrames++;
        }
        Check(held.Peek(0x8044)>=192,"held right did not repeat complete native hops");
        Check(landedFrames>=2,"visual hop lost its landing frames when ROM flag cleared");
        using var bottom=Started(rom);
        for(int f=0;f<20;f++)bottom.Step(InputPulse.ForHeldDirection(2,bottom.Snapshot()));
        Check(bottom.Peek(0x8047)==240&&FrogVisualState.PlayerOnBoard(bottom.Snapshot()),"ROM lower grass row is not visible or walkable");
        int bottomX=bottom.Peek(0x8044);
        for(int f=0;f<30;f++)bottom.Step(InputPulse.ForHeldDirection(8,bottom.Snapshot()));
        Check(bottom.Peek(0x8044)>bottomX,"frog cannot traverse the lower grass row");
        using var bounds=JsonDocument.Parse(File.ReadAllText("art/models.json"));
        var frogBounds=bounds.RootElement.EnumerateArray().Single(e=>e.GetProperty("asset").GetString()=="frog").GetProperty("footprintTiles");
        Check(Math.Abs(ModelFootprints.FrogAlongX-Math.Max(Math.Abs(frogBounds.GetProperty("minX").GetDouble()),Math.Abs(frogBounds.GetProperty("maxX").GetDouble()))*16)<.001,"frog X footprint drifted from Blender");
        Check(Math.Abs(ModelFootprints.FrogAcrossRow-Math.Max(Math.Abs(frogBounds.GetProperty("minY").GetDouble()),Math.Abs(frogBounds.GetProperty("maxY").GetDouble()))*16)<.001,"frog row footprint drifted from Blender");
        foreach(int lane in Enumerable.Range(6,5)){
            string model=lane switch{6=>"truck",7=>"sport",8=>"car",9=>"dozer",_=>"racecar"};
            var record=bounds.RootElement.EnumerateArray().Single(e=>e.GetProperty("asset").GetString()==model).GetProperty("footprintTiles");
            var footprint=ModelFootprints.Vehicle(lane);
            double minY=record.GetProperty("minY").GetDouble(),maxY=record.GetProperty("maxY").GetDouble();
            double expectedMin=(lane%2==0?minY:-maxY)*16*.84,expectedMax=(lane%2==0?maxY:-minY)*16*.84;
            double cross=Math.Max(Math.Abs(record.GetProperty("minX").GetDouble()),Math.Abs(record.GetProperty("maxX").GetDouble()))*16*.84;
            Check(Math.Abs(footprint.MinAlongX-expectedMin)<.001&&Math.Abs(footprint.MaxAlongX-expectedMax)<.001,"vehicle front/back bounds drifted from Blender: "+model);
            Check(Math.Abs(footprint.AcrossRow-cross)<.001,"vehicle width drifted from Blender: "+model);
            double leftEdge=footprint.MinAlongX-ModelFootprints.FrogAlongX,rightEdge=footprint.MaxAlongX+ModelFootprints.FrogAlongX;
            Check(ArcadeSimulation.ModelContact(leftEdge+.001,176,leftEdge+.001,176,0,0,176,footprint)&&ArcadeSimulation.ModelContact(rightEdge-.001,176,rightEdge-.001,176,0,0,176,footprint),"missing contact at authored front/back edge: "+model);
            Check(!ArcadeSimulation.ModelContact(leftEdge-.2,176,leftEdge-.2,176,0,0,176,footprint)&&!ArcadeSimulation.ModelContact(rightEdge+.2,176,rightEdge+.2,176,0,0,176,footprint),"vehicle collision exceeds model at front/back: "+model);
        }
        var car=ModelFootprints.Vehicle(8);
        Check(ArcadeSimulation.ModelContact(-40,160,40,160,0,0,160,car),"frog swept through car");
        Check(ArcadeSimulation.ModelContact(0,160,0,160,-40,40,160,car),"moving car swept through frog");
        Check(!ArcadeSimulation.ModelContact(-40,190,40,190,0,0,160,car),"collision ignores model row separation");
        foreach(int side in new[]{-1,1}){
            using var impact=Started(rom);impact.Modern(true);
            for(int lane=6;lane<=10;lane++)impact.Poke(0x8100+lane*9,0);
            impact.Poke(0x8148,1); // lane 8, the pink car lane at row 176
            impact.Poke(0x8149,132); // rendered car center 120
            impact.Poke(0x8044,120+side*12);impact.Poke(0x8047,176);
            impact.Poke(0x8004,0);impact.Poke(0x83cd,0);
            for(int a=0x8248;a<=0x8253;a++)impact.Poke(a,0);
            impact.Step();
            Check(impact.Peek(0x8004)!=0,$"model collision failed on {(side<0?"left":"right")} side of vehicle");
        }
        File.WriteAllText("docs/evidence/feedback-tests.json",JsonSerializer.Serialize(new{
            wrapping=true,deaths,bonuses,turtleMaximumStep=maxStep,turtleJumps=jumps,
            heldRightX=held.Peek(0x8044),lowerGrassRow=bottom.Peek(0x8047),
            nativeObjectMoves=nativeMoves,nativeObjectStalls=nativeStalls,subpixelMovesDuringNativeStalls=smoothedDuringStall,maxVisualLag,
            exportedModelBoundsVerified=6,vehicleFrontAndBackEdgesVerified=5,
            modelCollisionLiveSides=2
        },new JsonSerializerOptions{WriteIndented=true}));
        Console.WriteLine("Feedback regressions: wrapping, death/bonus continuity, smooth diving, held hops, both grass rows and model-bound road collision passed");
    }
    private static void Check(bool condition,string message){if(!condition)throw new Exception(message);}
}
