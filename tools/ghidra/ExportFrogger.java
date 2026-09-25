// Reproducible Z80 analysis and decompiler inventory. No model-generated success verdicts.
// @category Frogger
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.SourceType;
import java.nio.file.*;
import java.nio.charset.StandardCharsets;
import java.util.*;
import com.google.gson.*;

public class ExportFrogger extends GhidraScript {
  public void run() throws Exception {
    String[] args = getScriptArgs();
    Path out = Paths.get(args[0]);
    Files.createDirectories(out);
    boolean audio = args.length > 2 && args[2].equals("audio");
    int limit = audio ? 0x1800 : 0x3000;
    var memory = currentProgram.getMemory();
    memory.getBlock(toAddr(0)).setExecute(true);
    memory.getBlock(toAddr(0)).setWrite(false);
    if (memory.getBlock(toAddr(0x4000)) == null)
      memory.createUninitializedBlock("RAM_and_IO", toAddr(0x4000), 0xc000, false);
    TreeMap<Integer,String> entries = new TreeMap<>();
    entries.put(0, "reset");
    entries.put(audio ? 0x38 : 0x66, audio ? "audio_irq" : "vblank_nmi");
    if (Files.exists(Paths.get(args[1]))) {
      for (String line : Files.readAllLines(Paths.get(args[1]))) {
        String[] fields = line.split("\\t");
        entries.put(Integer.parseInt(fields[0],16), fields[1]);
      }
    }
    // Seed indirect-dispatch targets explicitly, retaining their provenance in seeds.tsv.
    for (var ent : entries.entrySet()) {
      Address a = toAddr(ent.getKey());
      if (ent.getKey() >= limit) continue;
      // A speculative sweep can start inside a real routine's first operand.
      // A declared entry must decode as an instruction, not a one-byte empty
      // function which DecompInterface misleadingly reports as "completed".
      if (getInstructionAt(a) == null) {
        Function stale = getFunctionAt(a);
        if (stale != null) removeFunction(stale);
        clearListing(a, a.add(2));
      }
      disassemble(a);
      createLabel(a, ent.getValue(), true);
    }
    analyzeAll(currentProgram);
    for (var ent : entries.entrySet()) {
      Address a = toAddr(ent.getKey());
      if (ent.getKey() >= limit) continue;
      Function fn = getFunctionAt(a);
      if (fn == null) fn = createFunction(a, ent.getValue());
      if (fn != null) fn.setName(ent.getValue(), SourceType.USER_DEFINED);
    }
    analyzeAll(currentProgram);
    DecompInterface decompiler = new DecompInterface();
    decompiler.openProgram(currentProgram);
    JsonArray functions = new JsonArray();
    StringBuilder asm = new StringBuilder();
    var instructions = currentProgram.getListing().getInstructions(true);
    int count = 0, bytes = 0;
    while (instructions.hasNext()) {
      Instruction ins = instructions.next();
      long address = ins.getAddress().getOffset();
      if (address >= limit) continue;
      StringBuilder hex = new StringBuilder();
      for (byte b : ins.getBytes()) hex.append(String.format("%02x", b & 255));
      asm.append(String.format("%04x\t%s\t%s\n", address, hex, ins.toString()));
      count++; bytes += ins.getLength();
    }
    var iterator = currentProgram.getFunctionManager().getFunctions(true);
    int passed = 0;
    while (iterator.hasNext()) {
      Function fn = iterator.next();
      if (fn.getEntryPoint().getOffset() >= limit) continue;
      DecompileResults r = decompiler.decompileFunction(fn, 60, monitor);
      boolean ok = r.decompileCompleted() && r.getDecompiledFunction() != null;
      JsonObject record = new JsonObject();
      String address = String.format("%04x", fn.getEntryPoint().getOffset());
      record.addProperty("address", address);
      record.addProperty("name", fn.getName());
      record.addProperty("body_bytes", fn.getBody().getNumAddresses());
      record.addProperty("decompiled", ok);
      record.addProperty("error", r.getErrorMessage());
      JsonArray callees = new JsonArray();
      for (Function callee : fn.getCalledFunctions(monitor)) callees.add(String.format("%04x", callee.getEntryPoint().getOffset()));
      record.add("callees", callees);
      if (ok) {
        Files.writeString(out.resolve(address + ".c"), r.getDecompiledFunction().getC(), StandardCharsets.UTF_8);
        passed++;
      }
      functions.add(record);
    }
    JsonObject report = new JsonObject();
    report.addProperty("processor", currentProgram.getLanguageID().toString());
    report.addProperty("program", currentProgram.getName());
    report.addProperty("instruction_count", count);
    report.addProperty("instruction_bytes", bytes);
    report.addProperty("function_count", functions.size());
    report.addProperty("decompiled_count", passed);
    report.add("functions", functions);
    JsonArray seedReport = new JsonArray();
    for (var ent : entries.entrySet()) {
      JsonObject s = new JsonObject();
      s.addProperty("address", String.format("%04x",ent.getKey()));
      s.addProperty("name", ent.getValue());
      Function fn = getFunctionContaining(toAddr(ent.getKey()));
      s.addProperty("containing_function", fn == null ? "" : String.format("%04x", fn.getEntryPoint().getOffset()));
      s.addProperty("instruction", getInstructionAt(toAddr(ent.getKey())) != null);
      seedReport.add(s);
    }
    report.add("seeds", seedReport);
    Files.writeString(out.resolve("inventory.json"), new GsonBuilder().setPrettyPrinting().create().toJson(report), StandardCharsets.UTF_8);
    Files.writeString(out.resolve("instructions.tsv"), asm, StandardCharsets.UTF_8);
    decompiler.dispose();
    println("FROGGER: " + passed + "/" + functions.size() + " decompiled; " + count + " instructions, " + bytes + " bytes");
  }
}
