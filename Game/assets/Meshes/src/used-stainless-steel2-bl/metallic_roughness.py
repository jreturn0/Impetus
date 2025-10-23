#!/usr/bin/env python3
"""
combine_metallic_roughness.py

Usage examples (run in folder with images):
  python combine_metallic_roughness.py
  python combine_metallic_roughness.py --dir "C:\textures" --b-source metallic
  python combine_metallic_roughness.py --skip-resize --preserve-alpha

Defaults:
 - Resize roughness to metallic size (can skip with --skip-resize)
 - B channel is black by default (change with --b-source)
 - Alpha is discarded by default; preserve with --preserve-alpha
"""

import os
import sys
import glob
import argparse
from PIL import Image

def normalize_open(path):
    """
    Open image and normalize to a Pillow Image object.
    We convert to RGBA first to ensure consistent handling (this drops embedded profiles).
    Return the opened Image (mode may be 'RGBA', 'RGB', or 'L'/'LA' depending on original).
    """
    img = Image.open(path)
    # Convert to a stable representation: keep alpha if present, else RGB
    if img.mode in ("1", "P", "CMYK", "YCbCr", "LAB"):  # various palette/modes -> convert
        # convert to RGBA to preserve alpha if any
        img = img.convert("RGBA")
    elif img.mode == "LA":  # grayscale + alpha
        # keep LA (we want to treat as grayscale+alpha)
        pass
    elif img.mode == "L":
        pass
    elif img.mode == "RGBA" or img.mode == "RGB":
        # already fine
        pass
    else:
        # fallback: convert to RGBA
        img = img.convert("RGBA")
    return img

def is_grayscale_mode(img):
    # If the mode is 'L' or 'LA' treat as grayscale (fast)
    if img.mode in ("L", "LA"):
        return True
    # If mode is RGB/RGBA, do a quick check to see if all channels equal on a small sample
    if img.mode in ("RGB", "RGBA"):
        # sample pixels (not the whole image) for speed
        w, h = img.size
        px = img.load()
        # sample up to 50 pixels spread across the image
        samples = []
        sx = max(1, w // 7)
        sy = max(1, h // 7)
        for y in range(0, h, sy):
            for x in range(0, w, sx):
                r, g, b = px[x, y][:3]
                if r != g or g != b:
                    return False
        # if we never found a differing pixel, treat as grayscale
        return True
    return False

def get_channel_image(img, requested):  # requested: 'R' or 'G' or 'B'
    """
    Return a single-channel 'L' image for the requested channel.
    If img is grayscale, return its gray channel.
    If img has alpha and user preserved alpha later, alpha is ignored here (we only return colour channel).
    """
    mode = img.mode
    if mode in ("L", "LA"):
        # grayscale: convert to 'L' if needed and return
        if mode == "LA":
            gray = img.convert("L")
        else:
            gray = img
        return gray
    # For RGB/RGBA:
    rgb = img.convert("RGB")
    r, g, b = rgb.split()
    if requested == 'R':
        return r
    if requested == 'G':
        return g
    if requested == 'B':
        return b
    raise ValueError("requested must be 'R','G' or 'B'")

def main(args):
    folder = args.dir
    pattern = os.path.join(folder, "*_metallic.png")
    metallic_files = sorted(glob.glob(pattern))
    if not metallic_files:
        print("No *_metallic.png files found in", folder)
        return

    for metal_path in metallic_files:
        base_name = os.path.basename(metal_path)
        base = base_name.rsplit("_metallic.png", 1)[0]
        rough_path = os.path.join(folder, f"{base}_roughness.png")
        if not os.path.exists(rough_path):
            print(f"Skipping '{base_name}': matching '{base}_roughness.png' not found.")
            continue

        try:
            metal_img = normalize_open(metal_path)
            rough_img = normalize_open(rough_path)
        except Exception as e:
            print(f"Error opening images for '{base}': {e}")
            continue

        # sizes
        msize = metal_img.size
        rsize = rough_img.size

        # Optionally resize roughness to match metallic size (default: resize)
        if msize != rsize:
            if args.skip_resize:
                print(f"Skipping '{base}': size mismatch {msize} vs {rsize} and --skip-resize set.")
                continue
            else:
                rough_img = rough_img.resize(msize, resample=Image.LANCZOS)

        # If grayscale detection is desired (we already account for 'L' modes above),
        # we will use the grayscale directly. We also treat RGB images with equal channels
        # as grayscale by checking is_grayscale_mode.
        # Extract R channel from metallic (or its gray channel)
        if is_grayscale_mode(metal_img):
            r_ch = metal_img.convert("L")
        else:
            r_ch = get_channel_image(metal_img, 'R')

        # Extract G channel from roughness (or its gray channel)
        if is_grayscale_mode(rough_img):
            g_ch = rough_img.convert("L")
        else:
            g_ch = get_channel_image(rough_img, 'G')

        # Determine B channel
        if args.b_source == "black":
            b_ch = Image.new("L", msize, 0)
        elif args.b_source == "metallic":
            if is_grayscale_mode(metal_img):
                b_ch = metal_img.convert("L")
            else:
                b_ch = get_channel_image(metal_img, 'B')
        elif args.b_source == "roughness":
            if is_grayscale_mode(rough_img):
                b_ch = rough_img.convert("L")
            else:
                b_ch = get_channel_image(rough_img, 'B')
        else:
            b_ch = Image.new("L", msize, 0)

        # Ensure channels are 'L' and same size
        r_ch = r_ch.resize(msize) if r_ch.size != msize else r_ch
        g_ch = g_ch.resize(msize) if g_ch.size != msize else g_ch
        b_ch = b_ch.resize(msize) if b_ch.size != msize else b_ch

        # Merge into RGB
        out_rgb = Image.merge("RGB", (r_ch, g_ch, b_ch))

        # Handle optional alpha preservation
        out_final = out_rgb
        if args.preserve_alpha:
            # Prefer metallic alpha, then roughness alpha, else no alpha
            alpha_source = None
            if metal_img.mode in ("LA", "RGBA"):
                alpha_source = metal_img
            elif rough_img.mode in ("LA", "RGBA"):
                alpha_source = rough_img

            if alpha_source is not None:
                # extract alpha channel and ensure size
                a = alpha_source.convert("RGBA").split()[3]
                if a.size != msize:
                    a = a.resize(msize, resample=Image.LANCZOS)
                out_final = out_rgb.convert("RGBA")
                out_final.putalpha(a)
            else:
                # no alpha available, just leave RGB
                out_final = out_rgb

        # Save
        out_name = f"{base}_metallic-roughness.png"
        out_path = os.path.join(folder, out_name)
        try:
            # Use optimized PNG saving
            out_final.save(out_path, optimize=True)
            print("Created:", out_path)
        except Exception as e:
            print(f"Failed saving '{out_name}': {e}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Combine R from *_metallic.png with G from *_roughness.png")
    parser.add_argument("--dir", "-d", default=".", help="Folder containing images (default: current folder)")
    parser.add_argument("--skip-resize", action="store_true", help="Skip pairs with mismatched sizes (instead of resizing roughness)")
    parser.add_argument("--preserve-alpha", action="store_true", help="If present, preserve alpha (from metallic if present, else roughness)")
    parser.add_argument("--b-source", choices=("black", "metallic", "roughness"), default="black",
                        help="Source for blue channel (default: black)")
    args = parser.parse_args()
    main(args)
