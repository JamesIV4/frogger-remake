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
