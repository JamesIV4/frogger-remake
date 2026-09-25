-- Independent function fixtures captured from MAME's actual Z80, never the port.
local cpu=manager.machine.devices[":maincpu"]
local mem=cpu.spaces["program"]
local dir=os.getenv("FROGGER_EVIDENCE")
local frame=0
local names={0x11bf,0x14b7,0x08e0,0x16f8,0x1cff}
local allregs={"AF","BC","DE","HL","IX","IY","SP"}
local active={}
_G.frogger_taps={}
local function statefile(path)
  local f=assert(io.open(path,"wb"));local chunks={}
  for _,range in ipairs({{0x8000,0x87ff},{0xa800,0xabff},{0xb000,0xb0ff}}) do
    for a=range[1],range[2] do chunks[#chunks+1]=string.char(mem:read_u8(a)) end
  end
  f:write(table.concat(chunks));f:close()
end
local function registers(path,pc,ret,sp)
  local f=assert(io.open(path,"w"));f:write(string.format("%d %d %d",pc,ret,sp))
  for _,r in ipairs(allregs) do f:write(" "..cpu.state[r].value) end
  f:write("\n");f:close()
end
for _,target in ipairs(names) do
  local key=string.format("%04x",target)
  local tap=mem:install_read_tap(target,target,"entry_"..key,function(offset,data,mask)
    if cpu.state["CURPC"].value~=target or active[target] or frame<270 then return data end
    if target==0x16f8 and mem:read_u8(0x8004)==0 then return data end
    if target==0x08e0 and cpu.state["DE"].value==0 then return data end
    local sp=cpu.state["SP"].value
    local ret=mem:read_u16(sp)
    active[target]=true
    statefile(dir.."/"..key.."-before.bin")
    registers(dir.."/"..key.."-before.txt",target,ret,sp)
    local complete=false
    local out=mem:install_read_tap(ret,ret,"return_"..key,function(o,d,m)
      if not complete and cpu.state["CURPC"].value==ret and cpu.state["SP"].value==(sp+2)%65536 then
        complete=true
        statefile(dir.."/"..key.."-after.bin")
        registers(dir.."/"..key.."-after.txt",ret,ret,sp+2)
        print("FROGGER_FIXTURE_COMPLETE "..key)
      end
      return d
    end)
    table.insert(_G.frogger_taps,out)
    return data
  end)
  table.insert(_G.frogger_taps,tap)
end
local ports=manager.machine.ioport.ports
_G.frogger_frame=emu.add_machine_frame_notifier(function()
 frame=frame+1
 ports[":IN0"].fields["Coin 1"]:set_value(frame>=150 and frame<158 and 1 or 0)
 ports[":IN1"].fields["1 Player Start"]:set_value(frame>=210 and frame<218 and 1 or 0)
 if frame==310 then mem:write_u8(0x8044,24);mem:write_u8(0x8047,32);mem:write_u8(0x8004,0);mem:write_u8(0x83cd,0) end
 if frame==500 then mem:write_u8(0x8004,1);mem:write_u8(0x83cd,0) end
end)
