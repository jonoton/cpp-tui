#!/usr/bin/env python3
"""Run a TUI binary in a pseudo-terminal of a fixed size and save its raw output.

Usage: capture.py BINARY COLS ROWS SECONDS OUT_FILE [KEYS_JSON]

KEYS_JSON (optional) is a JSON list of input steps, sent one at a time starting
1 second in and 0.25 seconds apart so the app handles each one separately:
  "text"                 sent in one write (JSON escapes work, e.g. "\\u001b[B" is Down)
  {"type": "text"}       typed one character at a time, like a person typing
  {"click": [col, row]}  a left mouse click at a 1-based terminal cell
  {"hover": [col, row], "for": seconds}
                         mouse movement (no button) at a cell, repeated every 0.2 s
                         for "for" seconds (default: once), e.g. to keep a tooltip up
  {"wait": seconds}      a pause before the next step
The program is sent "q" and then killed when SECONDS is up.

Besides OUT_FILE (all output bytes), OUT_FILE.timing.json records when each chunk
arrived as [[seconds, base64], ...], which render.mjs uses to make animated GIFs.
"""
import base64
import fcntl
import json
import os
import pty
import select
import struct
import sys
import termios
import time


def to_writes(step):
    """Turn one input step into a list of (bytes, seconds to wait afterwards)."""
    if isinstance(step, dict) and "click" in step:
        col, row = step["click"]
        return [(f"\x1b[<0;{col};{row}M\x1b[<0;{col};{row}m".encode(), 0.25)]
    if isinstance(step, dict) and "hover" in step:
        col, row = step["hover"]
        # SGR motion report with no button held: button code 3 (none) + 32 (motion).
        move = f"\x1b[<35;{col};{row}M".encode()
        return [(move, 0.2)] * max(1, round(float(step.get("for", 0)) / 0.2))
    if isinstance(step, dict) and "wait" in step:
        return [(b"", float(step["wait"]))]
    if isinstance(step, dict) and "type" in step:
        chars = [c.encode() for c in step["type"]]
        if not chars:
            return []
        return [(c, 0.06) for c in chars[:-1]] + [(chars[-1], 0.25)]
    return [(str(step).encode(), 0.25)]


def main() -> int:
    binary, cols, rows, secs, out = (
        sys.argv[1], int(sys.argv[2]), int(sys.argv[3]), float(sys.argv[4]), sys.argv[5])
    steps = []
    if len(sys.argv) > 6 and sys.argv[6]:
        steps = [w for k in json.loads(sys.argv[6]) for w in to_writes(k)]

    pid, fd = pty.fork()
    if pid == 0:
        # Size the terminal in the child, before exec, so the app's first size query
        # always sees the requested dimensions.
        fcntl.ioctl(sys.stdout.fileno(), termios.TIOCSWINSZ, struct.pack("HHHH", rows, cols, 0, 0))
        os.environ.update(TERM="xterm-256color", COLORTERM="truecolor", LANG="C.UTF-8")
        os.execv(binary, [binary])

    buf = bytearray()
    timing = []
    start = time.time()
    next_key_at = start + 1.0
    while time.time() - start < secs:
        ready, _, _ = select.select([fd], [], [], 0.05)
        if ready:
            try:
                chunk = os.read(fd, 65536)
            except OSError:
                break
            buf += chunk
            timing.append([round(time.time() - start, 4), base64.b64encode(chunk).decode()])
        if steps and time.time() >= next_key_at:
            data, wait = steps.pop(0)
            if data:
                os.write(fd, data)
            next_key_at += wait  # deadline scheduling: delays don't accumulate

    with open(out, "wb") as f:
        f.write(bytes(buf))
    with open(out + ".timing.json", "w") as f:
        json.dump(timing, f)
    try:
        os.write(fd, b"q")
        time.sleep(0.3)
        os.kill(pid, 9)
    except OSError:
        pass
    print(f"  captured {len(buf)} bytes from {os.path.basename(binary)}")
    return 0 if buf else 1


if __name__ == "__main__":
    sys.exit(main())
