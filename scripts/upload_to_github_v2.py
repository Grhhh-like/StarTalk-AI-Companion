# -*- coding: utf-8 -*-
"""
通过 GitHub Contents API 批量上传 StarTalk 仓库文件
- 使用 gh auth token 认证
- 处理已存在文件的 sha 更新
- 用 requests 避免命令行长度限制
"""
import base64
import json
import os
import subprocess
import sys

REPO = "Grhhh-like/StarTalk-AI-Companion"
BASE = r"C:\Users\颜研\DoubaoWork\chats\2026-09-15\new-chat\StarTalk-AI-Companion"
SKIP_DIRS = {".git", ".vscode", "__pycache__", "build", "managed_components"}

try:
    import requests
except ImportError:
    print("requests not installed, trying urllib...")
    requests = None

def get_token():
    r = subprocess.run(["gh", "auth", "token"], capture_output=True, text=True)
    return r.stdout.strip()

def get_sha(path):
    r = subprocess.run(
        ["gh", "api", f"repos/{REPO}/contents/{path}", "--jq", ".sha"],
        capture_output=True, text=True)
    out = r.stdout.strip()
    if out and "Not Found" not in out and "not found" not in out.lower():
        return out
    return None

def upload_urllib(token, rel, b64):
    import urllib.request
    sha = get_sha(rel)
    body = {"message": f"add/update {rel}", "content": b64}
    if sha:
        body["sha"] = sha
    url = f"https://api.github.com/repos/{REPO}/contents/{rel}"
    req = urllib.request.Request(url, method="PUT",
                                 data=json.dumps(body).encode(),
                                 headers={"Authorization": f"token {token}",
                                          "Accept": "application/vnd.github+json",
                                          "User-Agent": "StarTalk-Uploader"})
    try:
        with urllib.request.urlopen(req, timeout=60) as resp:
            resp.read()
            return True, ""
    except Exception as e:
        return False, str(e)[:300]

def upload_requests(token, rel, b64):
    sha = get_sha(rel)
    body = {"message": f"add/update {rel}", "content": b64}
    if sha:
        body["sha"] = sha
    url = f"https://api.github.com/repos/{REPO}/contents/{rel}"
    r = requests.put(url, json=body, headers={
        "Authorization": f"token {token}",
        "Accept": "application/vnd.github+json",
        "User-Agent": "StarTalk-Uploader",
    }, timeout=60)
    if r.status_code in (200, 201):
        return True, ""
    return False, f"HTTP {r.status_code}: {r.text[:200]}"

def collect_files(root):
    files = []
    for dirpath, dirnames, filenames in os.walk(root):
        dirnames[:] = [d for d in dirnames if d not in SKIP_DIRS]
        for fn in filenames:
            full = os.path.join(dirpath, fn)
            rel = os.path.relpath(full, root).replace("\\", "/")
            files.append((full, rel))
    return sorted(files, key=lambda x: x[1])

def main():
    token = get_token()
    if not token:
        print("no gh token")
        return 1
    print(f"token acquired ({token[:8]}...)")

    files = collect_files(BASE)
    print(f"total files: {len(files)}")
    ok, skip, fail = 0, 0, []
    for full, rel in files:
        with open(full, "rb") as f:
            b64 = base64.b64encode(f.read()).decode()
        if requests:
            success, err = upload_requests(token, rel, b64)
        else:
            success, err = upload_urllib(token, rel, b64)
        if success:
            ok += 1
            print(f"[OK] {rel}")
        else:
            fail.append((rel, err))
            print(f"[FAIL] {rel}: {err}")
    print(f"\nDone. success={ok}, skip={skip}, fail={len(fail)}")
    for rel, err in fail:
        print(f"  FAILED: {rel} -> {err}")
    return 1 if fail else 0

if __name__ == "__main__":
    sys.exit(main())
