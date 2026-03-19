# JUCE → React Connector Implementation

## What Was Built

### C++ Side

**`Source/ParameterBridge.h/.cpp`** — new bidirectional sync layer:
- Constructor registers as APVT listener on every parameter
- `parameterChanged()` dispatches to message thread and emits `parameterUpdate` var to JS
- `handleJSMessage()` receives knob changes from JS and calls `setValueNotifyingHost()`
- `sendFullState()` serializes all APVT values and pushes to JS on initial load

**`Source/PluginEditor.h/.cpp`** — replaced JUCE slider UI with:
- `WebBrowserComponent` with native integration enabled
- Two event listeners: `paramChange` (JS→C++) and `uiReady` (triggers full state push)
- Debug: loads `localhost:5173` (Vite HMR); Release: loads from `BinaryData`
- `ParameterBridge paramBridge` owns the sync logic

**`CMakeLists.txt`** — added `ParameterBridge.cpp`, `juce::juce_gui_extra`, and conditional `PluginUIData` binary data from `ui/dist/`

### React UI Side

| File | Purpose |
|---|---|
| `ui/src/bridge/juce.js` | JUCE runtime bridge; sets up `parameterUpdate`/`fullState` listeners, emits `uiReady` |
| `ui/src/bridge/mock.js` | Mock with default param values for browser-only development |
| `ui/src/bridge/index.js` | Auto-selects real or mock bridge based on `window.__JUCE__` |
| `ui/src/components/Knob.jsx` | Drag-to-rotate knob; syncs both ways via bridge |
| `ui/src/components/Toggle.jsx` | Checkbox mapped to bool APVT params |
| `ui/src/App.jsx` | All 6 parameters: bitDepth, reductionFactor, cutoff, dithering, linearInterp, polyBlep |

---

## How to Run the UI

### Option 1 — Browser (no JUCE, no plugin build needed)

Use this for fast UI iteration. The bridge automatically falls back to mock values.

```bash
cd ui
npm install       # first time only
npm run dev
```

Then open `http://localhost:5173` in a browser. Knobs and toggles are fully interactive with mock parameter state.

---

### Option 2 — Plugin Dev Mode (hot reload inside JUCE)

Run the Vite dev server alongside the plugin. The plugin's WebBrowserComponent loads from `localhost:5173`, so React changes appear instantly without rebuilding C++.

**Terminal 1:**
```bash
cd ui
npm run dev
```

**Terminal 2:**
```bash
cd /Users/dovydas/CLionProjects/demo
cmake -S . -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build cmake-build-debug
```

Open the built Standalone app or load the VST3 in a DAW. The UI loads from the Vite server automatically (Debug builds only).

---

### Option 3 — Release Build (UI embedded in binary)

The React app is compiled into the plugin as binary data — no external server needed.

```bash
# Step 1: build the React app
cd ui
npm run build          # produces ui/dist/

# Step 2: build the C++ plugin (picks up ui/dist/ as BinaryData)
cd ..
cmake -S . -B cmake-build-release -DCMAKE_BUILD_TYPE=Release
cmake --build cmake-build-release
```

The resulting VST3/Standalone contains the full UI with no runtime dependencies.

---

## Data Flow Summary

```
User drags knob
  → Knob.jsx calls setParameter("reductionFactor", 12.0)
  → bridge/juce.js emits window.__JUCE__.backend.emitEvent("paramChange", {...})
  → PluginEditor event listener fires
  → ParameterBridge::handleJSMessage() calls param->setValueNotifyingHost()
  → APVT updates → DSP reads new value next processBlock

DAW sends automation / user loads preset
  → APVT parameter changes
  → ParameterBridge::parameterChanged() fires
  → Dispatches to message thread
  → browser.emitEventIfBrowserIsVisible("parameterUpdate", var)
  → bridge/juce.js listener receives it
  → React Knob component re-renders with new value
```
