import hashlib
import json
import os
from pathlib import Path
import subprocess
import sys

group = sys.argv[1]
subdir = sys.argv[2]
os.environ.update(CONDA_SUBDIR=subdir, CONDA_SOLVER="libmamba", CONDA_CHANNEL_PRIORITY="strict")
local = Path("C:/rsvg-local")
conda = [sys.executable, "C:/rsvg-tools/Scripts/conda-script.py"]

def run(args):
    print(subprocess.list2cmdline([str(a) for a in args]), flush=True)
    subprocess.run(args, check=True)

def index():
    run(conda + ["index", str(local)])

groups = {"graphics": ["pygobject", "harfbuzz", "pango"], "buildtools": ["libgit2", "cargo-c"], "librsvg": ["librsvg"]}
for name in groups[group]:
    index()
    base = Path(".") if name == "librsvg" else Path("ci/parents") / name
    if name == "libgit2":
        run(["rattler-build", "build", "--recipe", str(base / "recipe"), "-m", str(base / "variants.yaml"),
             "--build-platform", subdir, "--target-platform", subdir, "--test", "native-and-emulated",
             "--output-dir", str(local), "--channel-priority", "strict", "-c", "file:///C:/rsvg-local", "-c", "conda-forge"])
    else:
        config = base / (f".ci_support/{subdir.replace('-', '_')}_.yaml" if name == "librsvg" else "variants.yaml")
        variants = {"target_platform": subdir, "build_platform": subdir, "c_compiler": "vs2022", "cxx_compiler": "vs2022", "python": "3.14.* *_cp314"}
        run(conda + ["build", str(base / "recipe"), "-m", str(config), "--variants", json.dumps(variants),
                     "--croot", f"C:/rsvg-build/{name}", "--output-folder", str(local), "--no-anaconda-upload", "--no-remove-work-dir",
                     "--override-channels", "-c", "file:///C:/rsvg-local", "-c", "conda-forge"])
    print(f"PASS: {name} build and package tests on {subdir}", flush=True)
index()
manifest = [{"file": p.name, "sha256": hashlib.sha256(p.read_bytes()).hexdigest()} for p in sorted((local / subdir).glob("*.conda"))]
(local / subdir / "sha256.json").write_text(json.dumps(manifest, indent=2))
