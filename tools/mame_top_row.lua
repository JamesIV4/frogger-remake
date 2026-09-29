-- Independent proof of the occupied-home early return on the top log row.
-- No ROM patches: only fixture RAM setup, with top-lane speed/phase stopped.
local cpu = manager.machine.devices[":maincpu"]
local mem = cpu.spaces["program"]
local dir = assert(os.getenv("FROGGER_EVIDENCE"))
local frame, index, pending = 0, 0, nil
local regs = {"AF", "BC", "DE", "HL", "IX", "IY", "SP"}
_G.top_row_taps = {}
local function capture(stem, pc, ret, sp)
  local file = assert(io.open(dir .. "/" .. stem .. ".bin", "wb"))
  local bytes = {}
  for _, range in ipairs({{0x8000,0x87ff},{0xa800,0xabff},{0xb000,0xb0ff}}) do
    for a = range[1], range[2] do bytes[#bytes+1] = string.char(mem:read_u8(a)) end
  end
  file:write(table.concat(bytes)); file:close()
  file = assert(io.open(dir .. "/" .. stem .. ".txt", "w"))
  file:write(string.format("%d %d %d", pc, ret, sp))
  for _, r in ipairs(regs) do file:write(" " .. cpu.state[r].value) end
  file:write("\n"); file:close()
end
table.insert(_G.top_row_taps, mem:install_read_tap(0x1cff,0x1cff,"top_row_entry",function(o,d,m)
  if cpu.state["CURPC"].value ~= 0x1cff or frame < 310 or pending or index >= 10 then return d end
  local player, bay = math.floor(index / 5) + 1, index % 5
  mem:write_u8(0x83fd,player)
  mem:write_u8(0x8044,24+48*bay); mem:write_u8(0x8047,48)
  mem:write_u8((player == 2 and 0x8263 or 0x825e)+bay,1)
  mem:write_u8(0x8004,0); mem:write_u8(0x83cd,0); mem:write_u8(0x8268,0)
  mem:write_u8(0x819b,0); mem:write_u8(0x81a6,0)
  for a=0x8248,0x8253 do mem:write_u8(a,0) end
  -- An in-flight left hop has five ticks left. The premature RET must not
  -- call the input/hop continuation routine, so this counter stays five.
  mem:write_u8(0x824b,1); mem:write_u8(0x8253,5)
  local sp = cpu.state["SP"].value
  local ret = mem:read_u16(sp)
  local stem = string.format("1cff-top-p%d-bay%d",player,bay)
  capture(stem .. "-before",0x1cff,ret,sp)
  pending = {stem=stem,ret=ret,sp=sp,input=false}
  return d
end))
table.insert(_G.top_row_taps, mem:install_read_tap(0x0000,0x2fff,"top_row_return",function(o,d,m)
  if pending then
    local pc = cpu.state["CURPC"].value
    if pc == 0x1acb then pending.input = true end
    if pc == pending.ret and cpu.state["SP"].value == (pending.sp+2)%65536 then
      assert(not pending.input and mem:read_u8(0x8253)==5,"Expected occupied-home input starvation")
      capture(pending.stem .. "-after",pending.ret,pending.ret,pending.sp+2)
      print("TOP_ROW_INPUT_SKIPPED " .. pending.stem)
      index=index+1; pending=nil
      if index==10 then manager.machine:exit() end
    end
  end
  return d
end))
local ports = manager.machine.ioport.ports
_G.top_row_frame = emu.add_machine_frame_notifier(function()
  frame=frame+1
  ports[":IN0"].fields["Coin 1"]:set_value(frame>=150 and frame<158 and 1 or 0)
  ports[":IN1"].fields["1 Player Start"]:set_value(frame>=210 and frame<218 and 1 or 0)
  if frame==310 then
    mem:write_u8(0x8044,120); mem:write_u8(0x8047,48)
    mem:write_u8(0x8004,0); mem:write_u8(0x83cd,0)
  end
end)
