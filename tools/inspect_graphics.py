from pathlib import Path
from PIL import Image, ImageDraw
root=Path(__file__).resolve().parents[1]
gfx=(root/'reference/assembled/gfx.bin').read_bytes()
colors=[(12,25,40),(255,219,127),(91,177,77),(226,105,71)]
out=Image.new('RGB',(16*48,16*58),(22,30,38));draw=ImageDraw.Draw(out)
for code in range(256):
    tile=Image.new('RGB',(8,8));px=tile.load()
    for y in range(8):
        for x in range(8):
            i=code*8+y;shift=7-x
            pen=(((gfx[i]>>shift)&1)<<1)|((gfx[2048+i]>>shift)&1)
            px[x,y]=colors[pen]
    tile=tile.rotate(-90).resize((40,40),Image.Resampling.NEAREST)
    x=(code%16)*48;y=(code//16)*58;out.paste(tile,(x,y));draw.text((x+4,y+42),f'{code:02x}',fill='white')
out.save(root/'docs/evidence/tiles.png')

# The top-row crocodile is a scrolling tile strip, not the full log slot.
# Decode its actual ROM table, including the two leading blank tile pairs.
rom=(root/'reference/assembled/maincpu.bin').read_bytes()
strip=Image.new('RGB',(64,16),colors[0]);mask=Image.new('L',(64,16))
for pair in range(8):
    for column in range(2):
        code=rom[0x1413+pair*2+column]
        tile=Image.new('RGB',(8,8));coverage=Image.new('L',(8,8))
        for y in range(8):
            for x in range(8):
                i=code*8+y;shift=7-x
                pen=(((gfx[i]>>shift)&1)<<1)|((gfx[2048+i]>>shift)&1)
                tile.putpixel((x,y),colors[pen]);coverage.putpixel((x,y),255 if pen else 0)
        position=(56-pair*8,column*8)
        strip.paste(tile.rotate(-90),position);mask.paste(coverage.rotate(-90),position)
bounds=mask.getbbox()
assert bounds==(0,0,47,16),bounds
destination=root/'docs/evidence/screenshots/river-croc-sprite.png'
destination.parent.mkdir(parents=True,exist_ok=True)
strip.resize((768,192),Image.Resampling.NEAREST).save(destination)
print('River crocodile: 64px tile strip, 47px occupied length, 16px head tile block; '+str(destination))
