import time

import sys
import subprocess
import pyte

import cv2
import numpy as np
from PIL import Image, ImageDraw, ImageFont

import serial
import serial.tools.list_ports
from threading import Thread

import psutil

procs: dict[int, psutil.Process] = {}


def feed():
    global procs
    for pid in psutil.pids():
        if pid not in procs or not procs[pid].is_running():
            try:
                procs[pid] = psutil.Process(pid)
            except Exception:
                pass

    out = (
        f"CPU:{psutil.cpu_percent():04.1f}  MEM:{psutil.virtual_memory().percent:04.1f}"
    )
    procmem = []
    for proc in procs.values():
        try:
            # procmem.append((proc.name(), proc.cpu_percent() / psutil.cpu_count()))
            procmem.append((proc.name(), proc.memory_percent()))
        except Exception:
            pass
    procmem = sorted(procmem, key=lambda x: x[1], reverse=True)
    for i in range(5):
        name, metric = procmem[i]
        name = name[:10]
        out += f"{name:>10}: {metric:04.1f}%\n"
    return out


ports = serial.tools.list_ports.comports()
kbport = None
for port, desc, hwid in sorted(ports):
    print(f"{port}: {desc} [{hwid}]")
    if desc == "Keyboard 0":
        kbport = port

assert kbport is not None
print("picked serial:", kbport)
ser = serial.Serial(kbport, baudrate=2_000_000)


def reader():
    while True:
        data = ser.read(1)
        sys.stdout.buffer.write(data)
        sys.stdout.flush()
        time.sleep(0.0)


# reader_thr = Thread(target=reader, daemon=True)
# reader_thr.start()

command = "date"

img_w, img_h = 128, 128
font = ImageFont.truetype("/Users/sagar/Library/Fonts/IosevkaNerdFont-Regular.ttf", 14)

bbox = font.getbbox("W")
cell_w, cell_h = bbox[2] - bbox[0], bbox[3] - bbox[1]

ascent, descent = font.getmetrics()
line_height = ascent + descent

cols = img_w // cell_w
rows = img_h // line_height

screen = pyte.Screen(cols, rows)

while True:
    data = feed()
    screen.reset()
    screen.set_mode(pyte.modes.LNM)

    stream = pyte.Stream(screen)
    stream.feed(data)

    img = Image.new("RGB", (img_w, img_h), "black")
    draw = ImageDraw.Draw(img)

    for ry, row in screen.buffer.items():
        for cx, char in row.items():
            ch = char.data or " "

            fg_rgb = (255, 255, 255)
            bg_rgb = (0, 0, 0)

            x0 = int(cx * cell_w)
            y0 = int(ry * line_height)
            x1 = int(x0 + cell_w)
            y1 = int(y0 + line_height)

            draw.rectangle([x0, y0, x1, y1], fill=bg_rgb)

            if ch.strip():
                draw.text((x0, y0), ch, font=font, fill=fg_rgb)

    img = np.array(img, dtype=np.uint8)
    img = cv2.threshold(img, 127, 255, cv2.THRESH_BINARY)[1]
    img = cv2.rotate(img, cv2.ROTATE_90_COUNTERCLOCKWISE)

    pixelbuf = []
    for j in range(16):
        page = []
        for i in range(128):
            col = 0
            for k in range(8):
                y = 8 * j + k
                x = i
                col |= int(img[y][x][0] == 255) << k
            page.append(col)
        pixelbuf.append(page)

    out = bytes()
    for page in pixelbuf:
        for col in page:
            out += col.to_bytes(1, "big")

    ser.write(b"\x00")
    for val in out:
        if val == 0 or val == 1:
            ser.write(b"\x01" + (0xFF - val).to_bytes(1))
        else:
            ser.write(val.to_bytes(1))

    time.sleep(0.01)

#     cv2.imshow("img", img)
#     if cv2.waitKey(1) == ord("q"):
#         break

# cv2.destroyAllWindows()
