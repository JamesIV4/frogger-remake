-- Capture consecutive calls to the actual ROM's beaver dispatcher (0x2b83).
-- FROGGER_EVIDENCE must name an existing output directory. This deliberately
-- stages level 3 and keeps the frog on the bank; creature/RNG/lane bytes are
-- otherwise left to the ROM. No native or JavaScript implementation is used.
local cpu = manager.machine.devices[":maincpu"]
local mem = cpu.spaces["program"]
local dir = assert(os.getenv("FROGGER_EVIDENCE"))
local limit = tonumber(os.getenv("FROGGER_BEAVER_SAMPLES")) or 2000
local regs = {"AF", "BC", "DE", "HL", "IX", "IY", "SP"}
local frame, count, pending = 0, 0, nil
_G.beaver_taps = {}
local function capture(stem, pc, ret, sp)
  local f = assert(io.open(dir .. "/" .. stem .. ".bin", "wb"))
  local bytes = {}
  for _, range in ipairs({{0x8000,0x87ff},{0xa800,0xabff},{0xb000,0xb0ff}}) do
    for a = range[1], range[2] do bytes[#bytes+1] = string.char(mem:read_u8(a)) end
  end
  f:write(table.concat(bytes)); f:close()
  f = assert(io.open(dir .. "/" .. stem .. ".txt", "w"))
  f:write(string.format("%d %d %d", pc, ret, sp))
  for _, r in ipairs(regs) do f:write(" " .. cpu.state[r].value) end
  f:write("\n"); f:close()
end
table.insert(_G.beaver_taps, mem:install_read_tap(0x2b83,0x2b83,"beaver_entry",function(o,d,m)
  if cpu.state["CURPC"].value ~= 0x2b83 or frame < 310 or pending or count >= limit then return d end
  mem:write_u8(0x83b7,3)
  mem:write_u8(0x8044,120); mem:write_u8(0x8047,224)
  mem:write_u8(0x83dd,200)
  count = count+1
  local sp = cpu.state["SP"].value
  local ret = mem:read_u16(sp)
  local stem = string.format("2b83-beaver-%04d",count)
  capture(stem .. "-before",0x2b83,ret,sp)
  pending = {stem=stem,ret=ret,sp=sp}
  return d
end))
-- The cluster returns here after dispatcher B. A single return tap avoids
-- leaving thousands of inactive taps installed during the long replay.
table.insert(_G.beaver_taps, mem:install_read_tap(0x0000,0x2fff,"beaver_return",function(o,d,m)
  if pending and cpu.state["CURPC"].value == pending.ret and cpu.state["SP"].value == (pending.sp+2)%65536 then
    capture(pending.stem .. "-after",pending.ret,pending.ret,pending.sp+2)
    pending = nil
    if count == limit then print("BEAVER_CAPTURE_COMPLETE " .. count); manager.machine:exit() end
  end
  return d
end))
local ports = manager.machine.ioport.ports
_G.beaver_frame = emu.add_machine_frame_notifier(function()
  frame = frame+1
  ports[":IN0"].fields["Coin 1"]:set_value(frame>=150 and frame<158 and 1 or 0)
  ports[":IN1"].fields["1 Player Start"]:set_value(frame>=210 and frame<218 and 1 or 0)
end)
