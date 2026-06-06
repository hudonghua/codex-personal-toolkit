from __future__ import annotations

from pathlib import Path
import math

import numpy as np
from PIL import Image, ImageDraw, ImageFont
from psd_tools import PSDImage
from psd_tools.constants import Compression


ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "diesel_engine_dashboard" / "grayscale_psd"
LAYERS = OUT / "vgus_static_layers"
W, H = 800, 480
SCALE = 4

STYLE = {
    "bg": (13, 13, 13, 255),
    "header": (34, 34, 34, 255),
    "nav": (7, 7, 7, 255),
    "card": (27, 27, 27, 255),
    "btn": (32, 32, 32, 255),
    "line": (140, 140, 140, 255),
    "line2": (74, 74, 74, 255),
    "text": (238, 238, 232, 255),
    "muted": (155, 155, 148, 255),
    "hi": (210, 210, 202, 255),
    "track": (62, 62, 60, 255),
    "dial": (8, 8, 8, 255),
    "dim": (100, 100, 96, 255),
    "pointer": (255, 158, 58, 255),
    "pointer_hi": (255, 215, 154, 255),
}

ORDER = [
    "background",
    "mono_engineering_label",
    "title_diesel",
    "tachometer_scale",
    "tachometer_pointer",
    "coolant_temp_box",
    "coolant_temp_label",
    "coolant_temp_unit",
    "oil_pressure_box",
    "oil_pressure_label",
    "oil_pressure_unit",
    "fuel_level_box",
    "fuel_level_label",
    "fuel_level_unit",
    "battery_voltage_box",
    "battery_voltage_label",
    "battery_voltage_unit",
    "engine_hours_box",
    "engine_hours_label",
    "engine_hours_unit",
    "fault_count_box",
    "fault_count_label",
    "fault_count_unit",
    "status_lamps_box",
    "status_lamps_labels",
    "nav_bar",
    "btn_home",
    "btn_left",
    "btn_down",
    "btn_up",
    "btn_ok",
    "btn_slot_1",
    "btn_slot_2",
    "btn_right",
]


def zh(text: str) -> str:
    return text.encode("ascii").decode("unicode_escape")


def font(name: str, size: int):
    try:
        return ImageFont.truetype(str(Path(r"C:\Windows\Fonts") / name), size)
    except OSError:
        return ImageFont.load_default()


F_TITLE = font("msyhbd.ttc", 26)
F_SMALL = font("msyh.ttc", 13)


def blank() -> Image.Image:
    return Image.new("RGBA", (W, H), (0, 0, 0, 0))


def save(name: str, image: Image.Image) -> None:
    LAYERS.mkdir(parents=True, exist_ok=True)
    image.save(LAYERS / f"{name}.png")


def centered(draw: ImageDraw.ImageDraw, xy, text: str, fnt, fill) -> None:
    x, y = xy
    box = draw.textbbox((0, 0), text, font=fnt)
    draw.text((x - (box[0] + box[2]) / 2, y - (box[1] + box[3]) / 2), text, font=fnt, fill=fill)


def panel(draw: ImageDraw.ImageDraw, box, fill=None, line=None, radius=6, width=1) -> None:
    draw.rounded_rectangle(box, radius, fill=fill or STYLE["card"], outline=line or STYLE["line2"], width=width)


def aa_layer(draw_func) -> Image.Image:
    im = Image.new("RGBA", (W * SCALE, H * SCALE), (0, 0, 0, 0))
    draw_func(ImageDraw.Draw(im), SCALE)
    return im.resize((W, H), Image.Resampling.LANCZOS)


def point(cx, cy, r, deg):
    a = math.radians(deg)
    return cx + math.cos(a) * r, cy + math.sin(a) * r


def make_background() -> None:
    im = Image.new("RGBA", (W, H), STYLE["bg"])
    draw = ImageDraw.Draw(im)
    draw.rectangle((0, 0, W, 54), fill=STYLE["header"])
    draw.line((0, 54, W, 54), fill=STYLE["line"], width=1)
    save("background", im)


def make_mono_label() -> None:
    im = blank()
    draw = ImageDraw.Draw(im)
    draw.text((18, 18), "MONO ENGINEERING", font=F_SMALL, fill=STYLE["muted"])
    save("mono_engineering_label", im)


def make_title() -> None:
    im = blank()
    draw = ImageDraw.Draw(im)
    centered(draw, (400, 27), zh(r"\u67f4\u6cb9\u673a\u5de5\u7a0b\u4eea\u8868"), F_TITLE, STYLE["text"])
    save("title_diesel", im)


def make_tachometer_scale() -> None:
    def draw_func(draw, s):
        x0, y0, x1, y1 = [v * s for v in (210, 74, 590, 340)]
        draw.rounded_rectangle((x0, y0, x1, y1), 8 * s, fill=STYLE["card"], outline=STYLE["line2"], width=s)
        cx, cy, r = 400 * s, 202 * s, 108 * s
        draw.ellipse((cx - r, cy - r, cx + r, cy + r), fill=STYLE["dial"], outline=STYLE["line2"], width=s)
        draw.arc((cx - r + 11 * s, cy - r + 11 * s, cx + r - 11 * s, cy + r - 11 * s), 135, 405, fill=STYLE["dim"], width=3 * s)
        for i in range(61):
            deg = 135 + 270 * i / 60
            major = i % 10 == 0
            mid = i % 5 == 0
            length = 24 if major else 15 if mid else 8
            x_start, y_start = point(cx, cy, r - 8 * s, deg)
            x_end, y_end = point(cx, cy, r - (8 + length) * s, deg)
            color = STYLE["text"] if major else STYLE["muted"] if mid else STYLE["dim"]
            draw.line((x_start, y_start, x_end, y_end), fill=color, width=(2 if major else 1) * s)

    im = aa_layer(draw_func)
    draw = ImageDraw.Draw(im)
    cx, cy = 400, 202
    for label, deg in [("0", 142), ("10", 222), ("20", 318), ("30", 398)]:
        x, y = point(cx, cy, 62, deg)
        centered(draw, (x, y), label, F_SMALL, STYLE["muted"])
    centered(draw, (cx, cy + 78), "RPM", F_SMALL, STYLE["muted"])
    save("tachometer_scale", im)


def make_tachometer_pointer() -> None:
    def draw_func(draw, s):
        cx, cy, r = 400 * s, 202 * s, 108 * s
        deg = 135 + 270 * 0.5
        tip = point(cx, cy, r - 42 * s, deg)
        left = point(cx, cy, 4 * s, deg + 108)
        right = point(cx, cy, 4 * s, deg - 108)
        tail = point(cx, cy, 16 * s, deg + 180)
        draw.polygon([left, right, tip], fill=STYLE["pointer"])
        draw.line((tail[0], tail[1], cx, cy), fill=STYLE["pointer"], width=2 * s)
        draw.line((cx, cy, tip[0], tip[1]), fill=STYLE["pointer_hi"], width=s)
        draw.ellipse((cx - 7 * s, cy - 7 * s, cx + 7 * s, cy + 7 * s), fill=STYLE["hi"], outline=STYLE["pointer"], width=2 * s)
        draw.ellipse((cx - 2 * s, cy - 2 * s, cx + 2 * s, cy + 2 * s), fill=STYLE["pointer"])

    im = aa_layer(draw_func)
    save("tachometer_pointer", im)
    im.save(OUT / "tachometer_pointer_fullcanvas.png")


def make_static_card(prefix: str, x: int, y: int, label: str, unit: str) -> None:
    box = blank()
    draw = ImageDraw.Draw(box)
    panel(draw, (x, y, x + 168, y + 58), STYLE["card"], STYLE["line2"], 5)
    draw.rounded_rectangle((x + 130, y + 31, x + 154, y + 42), 5, fill=STYLE["track"])
    save(f"{prefix}_box", box)

    label_layer = blank()
    draw = ImageDraw.Draw(label_layer)
    draw.text((x + 14, y + 8), label, font=F_SMALL, fill=STYLE["muted"])
    save(f"{prefix}_label", label_layer)

    unit_layer = blank()
    draw = ImageDraw.Draw(unit_layer)
    draw.text((x + 92, y + 38), unit, font=F_SMALL, fill=STYLE["muted"])
    save(f"{prefix}_unit", unit_layer)


def make_status_lamps() -> None:
    labels = [
        zh(r"\u673a\u6cb9"),
        zh(r"\u6c34\u6e29"),
        zh(r"\u5145\u7535"),
        zh(r"\u6ee4\u6e05"),
        zh(r"\u6545\u969c"),
    ]
    box_layer = blank()
    draw = ImageDraw.Draw(box_layer)
    for i, _ in enumerate(labels):
        x, y = 220 + i * 72, 360
        panel(draw, (x, y, x + 62, y + 26), STYLE["card"], STYLE["line"], 13)
        draw.ellipse((x + 8, y + 8, x + 18, y + 18), outline=STYLE["line"], width=1)
    save("status_lamps_box", box_layer)

    label_layer = blank()
    draw = ImageDraw.Draw(label_layer)
    for i, label in enumerate(labels):
        x, y = 220 + i * 72, 360
        draw.text((x + 24, y + 6), label, font=F_SMALL, fill=STYLE["text"])
    save("status_lamps_labels", label_layer)


def render_button(label: str, width: int, height: int) -> Image.Image:
    sw, sh = width * SCALE, height * SCALE
    im = Image.new("RGBA", (sw, sh), (0, 0, 0, 0))
    draw = ImageDraw.Draw(im)
    s = SCALE
    draw.rounded_rectangle((0, 0, sw - s, sh - s), 5 * s, fill=STYLE["btn"], outline=STYLE["line"], width=2 * s)
    centered(draw, (sw / 2, sh / 2), label, font("msyhbd.ttc", 23 * s), STYLE["text"])
    return im.resize((width, height), Image.Resampling.LANCZOS)


def make_nav() -> None:
    nav = blank()
    draw = ImageDraw.Draw(nav)
    draw.rectangle((0, 424, W, H), fill=STYLE["nav"])
    draw.line((0, 424, W, 424), fill=STYLE["line"], width=1)
    save("nav_bar", nav)
    for name, x, label in [
        ("btn_home", 1, "H"),
        ("btn_left", 102, "<"),
        ("btn_down", 203, "v"),
        ("btn_up", 304, "^"),
        ("btn_ok", 405, "OK"),
        ("btn_slot_1", 506, ""),
        ("btn_slot_2", 607, ""),
        ("btn_right", 708, ">"),
    ]:
        im = blank()
        im.alpha_composite(render_button(label, 91, 48), (x, 431))
        save(name, im)


def crop_to_alpha(image: Image.Image):
    alpha = np.array(image.getchannel("A"))
    ys, xs = np.nonzero(alpha)
    if len(xs) == 0:
        return None
    left, right = int(xs.min()), int(xs.max()) + 1
    top, bottom = int(ys.min()), int(ys.max()) + 1
    return image.crop((left, top, right, bottom)), left, top


def export_psd() -> Path:
    psd = PSDImage.new("RGBA", (W, H), color=(0, 0, 0, 0))
    for name in ORDER:
        image = Image.open(LAYERS / f"{name}.png").convert("RGBA")
        cropped = (image, 0, 0) if name == "background" else crop_to_alpha(image)
        if cropped is None:
            continue
        layer, left, top = cropped
        psd.create_pixel_layer(layer, name=name, top=top, left=left, compression=Compression.RAW)
    path = OUT / f"{zh(r'\u67f4\u6cb9\u673a\u4eea\u8868')}_VGUS{zh(r'\u9759\u6001\u5e95\u56fe')}.psd"
    psd.save(path)
    return path


def make_preview(path: Path) -> None:
    psd = PSDImage.open(path)
    psd.composite().convert("RGB").save(OUT / f"{zh(r'\u67f4\u6cb9\u673a\u4eea\u8868')}_VGUS{zh(r'\u9759\u6001\u5e95\u56fe')}_preview.png")


def main() -> None:
    OUT.mkdir(parents=True, exist_ok=True)
    make_background()
    make_mono_label()
    make_title()
    make_tachometer_scale()
    make_tachometer_pointer()
    make_static_card("coolant_temp", 24, 84, zh(r"\u6c34\u6e29"), zh(r"\u2103"))
    make_static_card("oil_pressure", 24, 164, zh(r"\u673a\u6cb9"), "kPa")
    make_static_card("fuel_level", 24, 244, zh(r"\u71c3\u6cb9"), "%")
    make_static_card("battery_voltage", 608, 84, zh(r"\u7535\u538b"), "V")
    make_static_card("engine_hours", 608, 164, zh(r"\u5c0f\u65f6"), "h")
    make_static_card("fault_count", 608, 244, zh(r"\u6545\u969c"), zh(r"\u4e2a"))
    make_status_lamps()
    make_nav()
    psd_path = export_psd()
    make_preview(psd_path)
    psd = PSDImage.open(psd_path)
    print(psd_path)
    print("layers", len(psd), "size", psd.size, "bytes", psd_path.stat().st_size)
    for layer in psd:
        print(layer.name, layer.bbox)


if __name__ == "__main__":
    main()
