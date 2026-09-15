# -*- coding: utf-8 -*-
"""
批量上传 StarTalk 仓库文件到 GitHub (通过 gh api contents API)
跳过 .git 目录
"""
import base64
import json
import os
import subprocess
import sys

REPO = "Grhhh-like/StarTalk-AI-Companion"
BASE = r"C:\Users\颜研\DoubaoWork\chats\2026-09-15\new-chat\StarTalk-AI-Companion"

SKIP_DIRS = {".git", ".vscode", "__pycache__", "build", "managed_components"}

def collect_files(root):
    files = []
    for dirpath, dirnames, filenames in os.walk(root):
        dirnames[:] = [d for d in dirnames if d not in SKIP_DIRS]
        for fn in filenames:
            full = os.path.join(dirpath, fn)
            rel = os.path.relpath(full, root).replace("\\", "/")
            files.append((full, rel))
    return sorted(files, key=lambda x: x[1])

def upload(full, rel):
    with open(full, "rb") as f:
        b64 = base64.b64encode(f.read()).decode()
    cmd = [
        "gh", "api", "--method", "PUT",
        f"repos/{REPO}/contents/{rel}",
        "-f", f"message=add {rel}",
        "-f", f"content={b64}",
        "-f", "branch=main",
    ]
    r = subprocess.run(cmd, capture_output=True, text=True)
    if r.returncode != 0:
        return False, r.stderr.strip()[:300]
    return True, ""

def main():
    files = collect_files(BASE)
    print(f"total files: {len(files)}")
    ok, fail = 0, []
    for full, rel in files:
        success, err = upload(full, rel)
        if success:
            ok += 1
            print(f"[OK] {rel}")
        else:
            fail.append((rel, err))
            print(f"[FAIL] {rel}: {err}")
    print(f"\nDone. success={ok}, fail={len(fail)}")
    for rel, err in fail:
        print(f"  FAILED: {rel} -> {err}")
    return 1 if fail else 0

if __name__ == "__main__":
    sys.exit(main())
