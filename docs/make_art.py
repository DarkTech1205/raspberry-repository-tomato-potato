"""Procedural original artwork (no image generator was available in the build sandbox).
Run:  python3 docs/make_art.py   -> writes resources/gcu-*.png"""
import math, random
from PIL import Image, ImageDraw, ImageFont, ImageFilter
random.seed(7)

def grad(w, h, c1, c2, diag=True):
    im = Image.new("RGB", (w, h)); px = im.load()
    for y in range(h):
        for x in range(w):
            t = ((x / w + y / h) / 2) if diag else y / h
            px[x, y] = tuple(int(c1[i] + (c2[i] - c1[i]) * t) for i in range(3))
    return im

def grid(d, w, h, step, col):
    for x in range(0, w, step): d.line([(x, 0), (x, h)], fill=col, width=1)
    for y in range(0, h, step): d.line([(0, y), (w, y)], fill=col, width=1)

def glow(im, blur=12):
    return Image.blend(im, im.filter(ImageFilter.GaussianBlur(blur)), .5)

def font(sz):
    for p in ("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", "/usr/share/fonts/dejavu/DejaVuSans-Bold.ttf"):
        try: return ImageFont.truetype(p, sz)
        except Exception: pass
    return ImageFont.load_default()

def spikes(d, w, h, col, n=12, base=None):
    base = base or h
    sw = w / n
    for i in range(n):
        d.polygon([(i * sw, base), ((i + .5) * sw, base - sw * .9), ((i + 1) * sw, base)], fill=col)

def cube(d, x, y, s, fill, line):
    d.rectangle([x, y, x + s, y + s], fill=fill, outline=line, width=max(3, s // 12))
    e = s // 5
    d.rectangle([x + e, y + e * 1.5, x + e * 2, y + e * 2.5], fill=(10, 8, 30))
    d.rectangle([x + e * 3, y + e * 1.5, x + e * 4, y + e * 2.5], fill=(10, 8, 30))
    d.rectangle([x + e, y + e * 3.3, x + e * 4, y + e * 3.8], fill=(10, 8, 30))

def save(im, name): im.save(f"resources/{name}.png"); print("wrote", name)

# --- backgrounds 512x384
def bg(name, c1, c2, accent, extra=None):
    im = grad(512, 384, c1, c2); d = ImageDraw.Draw(im, "RGBA")
    grid(d, 512, 384, 32, (255, 255, 255, 25))
    for _ in range(18):
        x, y, s = random.randint(0, 480), random.randint(0, 300), random.randint(14, 44)
        d.rectangle([x, y, x + s, y + s], outline=accent + (110,), width=2)
    spikes(d, 512, 384, accent + (160,), 16)
    if extra: extra(d)
    save(im, name)

bg("gcu-shop-bg", (255, 61, 129), (91, 44, 255), (255, 225, 77))
bg("gcu-mod-bg", (14, 19, 48), (27, 42, 107), (111, 209, 255),
   lambda d: [d.rectangle([0, 0, 512, 22], fill=(255, 210, 63, 200)) for _ in [0]])
bg("gcu-robtop-bg", (18, 8, 0), (58, 32, 0), (255, 176, 0),
   lambda d: [d.text((20, 40 + i * 28), "> SUDO MONETIZE --ALL" if i % 2 == 0 else "> LICENSE: $9.99/MO", font=font(18), fill=(255, 176, 0, 150)) for i in range(10)])
bg("gcu-pass-bg", (125, 44, 255), (255, 79, 163), (255, 225, 77))
bg("gcu-treasure-bg", (26, 11, 46), (120, 60, 10), (255, 210, 63))

# --- icons 128x128 (transparent)
def icon(name, fn):
    im = Image.new("RGBA", (128, 128), (0, 0, 0, 0)); d = ImageDraw.Draw(im); fn(d); save(im, name)
icon("gcu-gem", lambda d: (d.polygon([(64, 8), (116, 44), (64, 120), (12, 44)], fill=(255, 79, 163), outline=(160, 16, 95), width=6),
     d.line([(12, 44), (116, 44)], fill=(255, 208, 234), width=4), d.line([(40, 44), (64, 8), (88, 44), (64, 120), (40, 44)], fill=(255, 208, 234), width=3)))
icon("gcu-energy", lambda d: (d.polygon([(76, 6), (22, 76), (58, 76), (46, 122), (106, 46), (68, 46)], fill=(255, 225, 77), outline=(199, 138, 0), width=6)))
icon("gcu-cash", lambda d: (d.ellipse([8, 8, 120, 120], fill=(255, 210, 63), outline=(184, 134, 11), width=6), d.text((44, 30), "$", font=font(72), fill=(122, 82, 0))))

# --- logo 512x256
def logo():
    im = Image.new("RGBA", (512, 256), (0, 0, 0, 0)); d = ImageDraw.Draw(im)
    cube(d, 20, 60, 120, (67, 233, 123), (255, 255, 255))
    d.text((160, 40), "HOW TO", font=font(52), fill=(255, 255, 255)); d.text((160, 96), "RUIN GD", font=font(78), fill=(255, 225, 77))
    d.text((165, 190), "every button costs $", font=font(26), fill=(255, 79, 163))
    save(im, "gcu-logo")
logo()

# --- promo 512x320 advert
def promo():
    im = grad(512, 320, (255, 61, 129), (91, 44, 255)); d = ImageDraw.Draw(im, "RGBA")
    grid(d, 512, 320, 32, (255, 255, 255, 30))
    cube(d, 40, 120, 110, (67, 233, 123), (255, 255, 255))
    d.rounded_rectangle([190, 70, 490, 250], 18, fill=(11, 7, 32), outline=(255, 225, 77), width=5)
    d.text((215, 85), "MYTHIC CUBE", font=font(40), fill=(255, 79, 163))
    d.text((215, 140), "ONLY $59.99", font=font(48), fill=(255, 255, 255))
    d.text((215, 205), "limited: 1 rotation", font=font(24), fill=(255, 225, 77))
    spikes(d, 512, 320, (11, 7, 32, 230), 16)
    save(im, "gcu-promo")
promo()

# --- premium cosmetic showcase 256
def premium():
    im = Image.new("RGBA", (256, 256), (0, 0, 0, 0)); d = ImageDraw.Draw(im)
    for r in range(120, 20, -6): d.ellipse([128 - r, 128 - r, 128 + r, 128 + r], fill=(255, 176, 0, int(5 + (120 - r) * .7)))
    cube(d, 64, 64, 128, (255, 79, 163), (255, 225, 77))
    for a in range(0, 360, 30):
        x, y = 128 + 112 * math.cos(math.radians(a)), 128 + 112 * math.sin(math.radians(a))
        d.polygon([(x, y - 9), (x + 4, y), (x, y + 9), (x - 4, y)], fill=(255, 245, 160))
    save(im, "gcu-premium-cosmetic")
premium()
