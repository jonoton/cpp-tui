// Render captured terminal output (ANSI bytes) with xterm.js in headless Chromium.
// Usage:
//   node render.mjs IN_FILE OUT.png COLS ROWS FONT_SIZE
//   node render.mjs IN_FILE OUT.gif COLS ROWS FONT_SIZE FROM_S TO_S FPS [HOLD_S]
// PNG mode renders the final screen at 2x. GIF mode replays IN_FILE.timing.json
// (written by capture.py) and records the screen between FROM_S and TO_S, pausing
// HOLD_S seconds on the last frame before it loops.
import { readFileSync, writeFileSync, existsSync } from 'node:fs';
import { createRequire } from 'node:module';
import { dirname, join } from 'node:path';
import { tmpdir } from 'node:os';
import { chromium } from 'playwright-core';

const require = createRequire(import.meta.url);
const [inFile, outFile, cols, rows, fontSize, gifFrom, gifTo, gifFps, gifHold] = process.argv.slice(2);
const isGif = outFile.endsWith('.gif');

const xtermJs = readFileSync(require.resolve('@xterm/xterm/lib/xterm.js'), 'utf8');
const xtermCss = readFileSync(require.resolve('@xterm/xterm/css/xterm.css'), 'utf8');
const unicode11Js = readFileSync(require.resolve('@xterm/addon-unicode11/lib/addon-unicode11.js'), 'utf8');
// The canvas renderer draws box-drawing, block and Braille characters itself
// (customGlyphs), so borders and charts join up without gaps between cells.
const canvasJs = readFileSync(require.resolve('@xterm/addon-canvas/lib/addon-canvas.js'), 'utf8');

function findChromium() {
  const candidates = [
    process.env.CHROMIUM_PATH,
    '/usr/bin/chromium',
    '/usr/bin/chromium-browser',
    '/usr/bin/google-chrome',
    '/usr/bin/google-chrome-stable',
    '/snap/bin/chromium',
    '/Applications/Google Chrome.app/Contents/MacOS/Google Chrome',
    '/Applications/Chromium.app/Contents/MacOS/Chromium',
  ].filter(Boolean);
  const found = candidates.find((p) => existsSync(p));
  if (found) return found;
  // Fall back to a browser installed with `npx playwright-core install chromium`.
  try {
    const bundled = chromium.executablePath();
    if (bundled && existsSync(bundled)) return bundled;
  } catch {}
  throw new Error('Chromium not found. Set CHROMIUM_PATH or run `npx playwright-core install chromium`.');
}

// Fonts come from npm (@fontsource) and are inlined, so rendering needs no network
// and looks the same on every machine. Noto covers symbols and emoji that
// JetBrains Mono lacks (e.g. the ⏲ and ⚡ glyphs in htop_sim).
function inlineFontCss(pkgCss, extra = '') {
  const file = require.resolve(pkgCss);
  return readFileSync(file, 'utf8')
    .replace(/font-display:/g, `${extra}font-display:`)
    .replace(/,\s*url\([^)]*\.woff\)\s*format\('woff'\)/g, '')
    .replace(/url\(\.\/files\/([^)]+\.woff2)\)/g, (_, f) =>
      `url(data:font/woff2;base64,${readFileSync(join(dirname(file), 'files', f)).toString('base64')})`)
    .replace(/font-display:\s*swap/g, 'font-display: block');
}
const fontCss = [
  '@fontsource/jetbrains-mono/400.css',
  '@fontsource/jetbrains-mono/700.css',
  // Noto Sans Symbols 2 draws some single-cell symbols (e.g. ● in RadioSet) wider
  // than a JetBrains Mono cell; shrink it so they stay inside their cell.
  ['@fontsource/noto-sans-symbols-2/400.css', 'size-adjust: 80%; '],
  '@fontsource/noto-emoji/400.css',
].map((f) => (Array.isArray(f) ? inlineFontCss(...f) : inlineFontCss(f))).join('\n');

const html = `<!DOCTYPE html><html><head><meta charset="utf-8"><style>${fontCss}
${xtermCss}
  html, body { margin: 0; background: transparent; }
  /* Rounded corners are baked into the PNG (transparent outside the curve), so they
     show up the same in GitHub, VS Code and other Markdown previews that ignore CSS. */
  #term { display: inline-block; padding: calc(14px * var(--s, 1)) calc(16px * var(--s, 1));
    background: #1e1e2e; border-radius: calc(14px * var(--s, 1)); overflow: hidden; }
  .xterm-viewport { overflow: hidden !important; }
</style></head><body><div id="term"></div><script>${xtermJs}</script><script>${unicode11Js}</script>
<script>${canvasJs}</script></body></html>`;

// Chromium's profile socket path must fit in 108 bytes; fall back to /tmp when
// the system temp directory path is too long.
if (tmpdir().length > 60 && existsSync('/tmp')) process.env.TMPDIR = '/tmp';

const browser = await chromium.launch({ executablePath: findChromium(), args: ['--no-sandbox'] });
// PNGs render at 2x by doubling the font size at a device scale of 1, so the canvas
// renderer draws every cell at full resolution. GIFs stay at 1x to keep files small.
const SCALE = isGif ? 1 : 2;
const page = await browser.newPage({ deviceScaleFactor: 1, viewport: { width: 4000, height: 3000 } });
await page.setContent(html);
await page.evaluate(async ({ cols, rows, fontSize, scale }) => {
  document.documentElement.style.setProperty('--s', scale);
  // Make sure the fonts are loaded before xterm.js measures the cell size.
  await Promise.all(['400 16px "JetBrains Mono"', '700 16px "JetBrains Mono"',
    '16px "Noto Sans Symbols 2"', '16px "Noto Emoji"'].map((f) => document.fonts.load(f, '⏲⚡●A')));
  await document.fonts.ready;
  const term = new Terminal({
    cols, rows, fontSize,
    fontFamily: '"JetBrains Mono", "Noto Sans Symbols 2", "Noto Emoji", monospace',
    lineHeight: 1.0,
    allowProposedApi: true,
    customGlyphs: true,
    // Fallback-font glyphs wider than a cell (e.g. ● in RadioSet) are scaled to fit.
    rescaleOverlappingGlyphs: true,
    allowTransparency: false,
    cursorBlink: false,
    theme: { background: '#1e1e2e' },
  });
  // Use Unicode 11 width tables (like modern terminals) so wide glyphs such as
  // emoji take two cells, matching cpp-tui's own width calculation.
  term.loadAddon(new Unicode11Addon.Unicode11Addon());
  term.unicode.activeVersion = '11';
  term.open(document.getElementById('term'));
  term.loadAddon(new CanvasAddon.CanvasAddon());
  window.writeChunks = async (chunks) => {
    for (const b64 of chunks) {
      const bytes = Uint8Array.from(atob(b64), (c) => c.charCodeAt(0));
      await new Promise((resolve) => term.write(bytes, resolve));
    }
    // Match the padding around the terminal to whatever the app painted in the top-left cell.
    const cell = term.buffer.active.getLine(0)?.getCell(0);
    if (cell && cell.isBgRGB()) {
      document.getElementById('term').style.background = '#' + cell.getBgColor().toString(16).padStart(6, '0');
    }
    await new Promise((r) => requestAnimationFrame(() => requestAnimationFrame(r)));
  };
}, { cols: +cols, rows: +rows, fontSize: +fontSize * SCALE, scale: SCALE });

const termEl = page.locator('#term');
if (!isGif) {
  await page.evaluate((d) => window.writeChunks([d]), readFileSync(inFile).toString('base64'));
  await page.waitForTimeout(300);
  await termEl.screenshot({ path: outFile, omitBackground: true });
} else {
  await renderGif();
}
await browser.close();
console.log(`  wrote ${outFile}`);

async function renderGif() {
  const { GIFEncoder, quantize, applyPalette } = require('gifenc');
  const { PNG } = require('pngjs');
  const timing = JSON.parse(readFileSync(inFile + '.timing.json', 'utf8'));
  const from = +gifFrom, to = +gifTo, fps = +gifFps;
  const delay = Math.round(1000 / fps);

  // Grab frames: feed the output that had arrived by each frame time, then screenshot.
  const frames = [];
  let next = 0;
  for (let t = 0; t <= to + 1e-9; t += 1 / fps) {
    const chunks = [];
    while (next < timing.length && timing[next][0] <= t) chunks.push(timing[next++][1]);
    if (chunks.length || t >= from) await page.evaluate((c) => window.writeChunks(c), chunks);
    if (t >= from - 1e-9) frames.push(PNG.sync.read(await termEl.screenshot({ omitBackground: true })));
  }
  const { width, height } = frames[0];

  // One shared palette (sampled from up to 12 frames spread over the recording, so
  // every theme or state shown gets its colours) avoids colour flicker. Index 255
  // is reserved as the transparent colour, used for the rounded corners and for
  // pixels that did not change since the previous frame (which keeps files small).
  const picks = Math.min(12, frames.length);
  const sample = Array.from({ length: picks }, (_, k) =>
    frames[Math.round((k * (frames.length - 1)) / Math.max(1, picks - 1))].data);
  const merged = new Uint8Array(sample.reduce((n, d) => n + d.length, 0));
  sample.reduce((o, d) => (merged.set(d, o), o + d.length), 0);
  const palette = quantize(merged, 255, { format: 'rgb565' });
  while (palette.length < 255) palette.push([0, 0, 0]);
  palette.push([0, 0, 0]);
  const TRANSPARENT = 255;

  const gif = GIFEncoder();
  let prev = null;
  const hold = Math.round(1000 * (+gifHold || 0));
  for (const [i, frame] of frames.entries()) {
    const idx = applyPalette(frame.data, palette.slice(0, 255), 'rgb565');
    const out = new Uint8Array(idx);
    for (let p = 0; p < out.length; p++) {
      if (frame.data[p * 4 + 3] < 128) out[p] = TRANSPARENT;
      else if (prev && prev[p] === idx[p]) out[p] = TRANSPARENT;
    }
    gif.writeFrame(out, width, height, {
      palette, delay: i === frames.length - 1 ? Math.max(delay, hold) : delay, transparent: true, transparentIndex: TRANSPARENT, dispose: 1, repeat: 0,
    });
    prev = idx;
  }
  gif.finish();
  writeFileSync(outFile, gif.bytes());
}
