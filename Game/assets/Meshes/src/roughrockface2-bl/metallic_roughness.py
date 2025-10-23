#!/usr/bin/env python3
"""
combine_metallic_roughness_robust_case.py

Features:
- Case-insensitive matching of _metallic.png and _roughness.png
- Grayscale detection
- Optional alpha preservation
- Optional resizing of roughness
- B channel choice
- Robust error handling per-file and global with log
- Safe to run by double-clicking (window stays open)
"""
from PIL import Image, ImageOps
import os, glob, argparse, traceback, datetime
from PIL import Image, ImageFile

ImageFile.LOAD_TRUNCATED_IMAGES = True

from PIL import Image

def safe_open(path):
    """Open image robustly and normalize to 8-bit Pillow Image."""
    img = Image.open(path)

    # Palette / indexed / exotic modes -> convert to RGBA
    if img.mode in ("1", "P", "CMYK", "YCbCr", "LAB"):
        img = img.convert("RGBA")

    # 16-bit grayscale or RGB
    elif img.mode in ("I;16", "I;16B", "I;16L", "I"):
        # Convert to 16-bit integer first
        img = img.convert("I")
        # Scale 16-bit 0-65535 down to 8-bit 0-255 safely
        # Convert to a list of pixels, then back
        pixels = list(img.getdata())
        pixels_8bit = [p * 255 // 65535 for p in pixels]
        img = Image.new("L", img.size)
        img.putdata(pixels_8bit)

    # Already 8-bit or standard
    elif img.mode in ("L", "LA", "RGB", "RGBA"):
        pass

    # Anything else -> fallback
    else:
        img = img.convert("RGBA")

    return img



def is_grayscale(img):
    if img.mode in ("L", "LA"):
        return True
    rgb = img.convert("RGB")
    w, h = rgb.size
    px = rgb.load()
    for y in range(0, h, max(1, h // 8)):
        for x in range(0, w, max(1, w // 8)):
            r, g, b = px[x, y]
            if r != g or g != b:
                return False
    return True

def get_channel(img, channel):
    if img.mode in ("L", "LA"):
        return img.convert("L")
    r, g, b = img.convert("RGB").split()
    if channel == "R": return r
    if channel == "G": return g
    if channel == "B": return b
    raise ValueError("Invalid channel")

def combine_channels(r_img, g_img, b_img):
    return Image.merge("RGB", (r_img, g_img, b_img))

def process_pair(metal_path, rough_path, args):
    try:
        metal_img = safe_open(metal_path)
        rough_img = safe_open(rough_path)

        if metal_img.size != rough_img.size:
            if args.skip_resize:
                print(f"[SKIP] {os.path.basename(metal_path)}: size mismatch, skipping")
                return
            rough_img = rough_img.resize(metal_img.size, Image.LANCZOS)

        r_ch = metal_img.convert("L") if is_grayscale(metal_img) else get_channel(metal_img, "R")
        g_ch = rough_img.convert("L") if is_grayscale(rough_img) else get_channel(rough_img, "G")

        if args.b_source == "black":
            b_ch = Image.new("L", metal_img.size, 0)
        elif args.b_source == "metallic":
            b_ch = metal_img.convert("L") if is_grayscale(metal_img) else get_channel(metal_img, "B")
        elif args.b_source == "roughness":
            b_ch = rough_img.convert("L") if is_grayscale(rough_img) else get_channel(rough_img, "B")
        else:
            b_ch = Image.new("L", metal_img.size, 0)

        out_rgb = combine_channels(r_ch, g_ch, b_ch)

        out_final = out_rgb
        if args.preserve_alpha:
            alpha_source = metal_img if metal_img.mode in ("LA", "RGBA") else rough_img if rough_img.mode in ("LA", "RGBA") else None
            if alpha_source:
                a = alpha_source.convert("RGBA").split()[3]
                if a.size != metal_img.size:
                    a = a.resize(metal_img.size, Image.LANCZOS)
                out_final = out_rgb.convert("RGBA")
                out_final.putalpha(a)

        base = os.path.basename(metal_path).rsplit("_", 1)[0]
        out_name = f"{base}_metallic-roughness.png"
        out_path = os.path.join(os.path.dirname(metal_path), out_name)
        out_final.save(out_path, optimize=True)
        print(f"[OK] Created: {out_path}")
    except Exception as e:
        print(f"[ERROR] Failed processing {metal_path} / {rough_path}: {e}")

def main(args):
    folder = args.dir
    metallic_files = sorted(glob.glob(os.path.join(folder, "*[Mm]etallic.png")))
    if not metallic_files:
        print("No metallic files found in", folder)
        return

    roughness_files = {os.path.basename(f).lower(): f for f in glob.glob(os.path.join(folder, "*[Rr]oughness.png"))}

    for metal_path in metallic_files:
        base_lower = os.path.basename(metal_path).lower().rsplit("_metallic.png", 1)[0]
        rough_candidate = f"{base_lower}_roughness.png"
        if rough_candidate not in roughness_files:
            print(f"[SKIP] {metal_path}: matching roughness file not found")
            continue
        rough_path = roughness_files[rough_candidate]
        process_pair(metal_path, rough_path, args)

if __name__ == "__main__":
    try:
        parser = argparse.ArgumentParser(description="Combine R from metallic with G from roughness (case-insensitive, robust)")
        parser.add_argument("--dir", "-d", default=".", help="Folder containing images")
        parser.add_argument("--skip-resize", action="store_true", help="Skip size-mismatched images")
        parser.add_argument("--preserve-alpha", action="store_true", help="Preserve alpha channel if present")
        parser.add_argument("--b-source", choices=("black", "metallic", "roughness"), default="black", help="Source for B channel")
        args = parser.parse_args()
        main(args)
    except Exception as e:
        logname = f"combine_error_{datetime.datetime.now().strftime('%Y%m%d_%H%M%S')}.log"
        with open(logname, "w", encoding="utf-8") as fh:
            fh.write("Unhandled exception:\n")
            fh.write(traceback.format_exc())
        print(f"[FATAL] Unhandled exception: {e}")
        print(f"Traceback written to {logname}")
        input("Press Enter to exit...")
    else:
        print("\nProcessing finished successfully.")
        input("Press Enter to close...")
