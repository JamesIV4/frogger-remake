-- Capture the actual ROM's back, neck and snout decisions, independent of the port.
local cpu = manager.machine.devices[":maincpu"]
local mem = cpu.spaces["program"]
local dir = assert(os.getenv("FROGGER_EVIDENCE"))
local frame, index = 0, 1
local cases = {{"back", 110}, {"bite", 130}, {"snout", 152}}
local regs = {"AF", "BC", "DE", "HL", "IX", "IY", "SP"}
local pending = false
_G.river_taps = {}
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
table.insert(_G.river_taps, mem:install_read_tap(0x28bb,0x28bb,"river_entry",function(o,d,m)
  if cpu.state["CURPC"].value ~= 0x28bb or frame < 310 or pending or index > #cases then return d end
  local case = cases[index]
  mem:write_u8(0x8150,1); mem:write_u8(0x83b7,2)
  mem:write_u8(0x8044,case[2]); mem:write_u8(0x8047,48); mem:write_u8(0x8101,160)
  mem:write_u8(0x8004,0); mem:write_u8(0x829c,0); mem:write_u8(0x83cd,0)
  local sp = cpu.state["SP"].value
  local ret = mem:read_u16(sp)
  local stem = "28bb-" .. case[1]
  capture(stem .. "-before",0x28bb,ret,sp)
  pending = true
  local complete = false
  table.insert(_G.river_taps,mem:install_read_tap(ret,ret,stem,function(a,b,c)
    if not complete and cpu.state["CURPC"].value == ret and cpu.state["SP"].value == (sp+2)%65536 then
      complete = true
      capture(stem .. "-after",ret,ret,sp+2)
      print(string.format("RIVER %s death=%d drown=%d",case[1],mem:read_u8(0x8004),mem:read_u8(0x829c)))
      index = index+1; pending = false
    end
    return b
  end))
  return d
end))
local ports = manager.machine.ioport.ports
_G.river_frame = emu.add_machine_frame_notifier(function()
  frame = frame+1
  ports[":IN0"].fields["Coin 1"]:set_value(frame>=150 and frame<158 and 1 or 0)
  ports[":IN1"].fields["1 Player Start"]:set_value(frame>=210 and frame<218 and 1 or 0)
end)
