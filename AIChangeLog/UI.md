# JUCE + React Plugin UI — Implementation Plan

## Architecture Overview

```
┌─────────────────────────────────────┐
│         React UI (WebView)          │
│   Knobs, visualizers, controls      │
│         ↕ window.__JUCE__           │
├─────────────────────────────────────┤
│       ParameterBridge (C++)         │
│  Owns APVT listeners, serializes   │
│  param state to/from JSON           │
│         ↕ APVT listeners           │
├─────────────────────────────────────┤
│   AudioProcessor / Bitcrusher DSP   │
│         ↕                           │
│   DAW Host (automation, presets,    │
│   state save/restore)               │
└─────────────────────────────────────┘
```

The `AudioProcessorValueTreeState` (APVT) is the **single source of truth** for all parameters. The React UI is a subscriber — it never writes directly to DSP state. The DAW host, preset system, and UI all interact exclusively through the APVT.

---

## Project Layout

```
my-plugin/
├── Source/
│   ├── PluginProcessor.h/cpp        # DSP + APVT ownership
│   ├── PluginEditor.h/cpp           # WebBrowserComponent setup
│   ├── ParameterBridge.h/cpp        # NEW — bidirectional param sync
│   └── Bitcrusher.h/cpp             # DSP implementation
├── ui/
│   ├── src/
│   │   ├── main.jsx                 # Entry point
│   │   ├── App.jsx                  # Root component
│   │   ├── components/
│   │   │   └── Knob.jsx             # Reusable knob component
│   │   └── bridge/
│   │       ├── juce.js              # Bridge abstraction layer
│   │       └── mock.js              # Mock bridge for browser dev
│   ├── dist/                        # Built output (gitignored)
│   ├── package.json
│   └── vite.config.js
├── Resources/                       # BinaryData source for release builds
└── CMakeLists.txt
```

---

## Phase 1: C++ Foundation

### 1.1 — ParameterBridge Class

This is the critical new piece. It sits between the APVT and the WebBrowserComponent, handling all serialization and message routing.

**ParameterBridge.h**
```cpp
#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_extra/juce_gui_extra.h>

class ParameterBridge : public juce::AudioProcessorValueTreeState::Listener
{
public:
    ParameterBridge(juce::AudioProcessorValueTreeState& apvt,
                    juce::WebBrowserComponent& browser);
    ~ParameterBridge() override;

    // APVT listener — fires when any parameter changes (host, preset, etc.)
    void parameterChanged(const juce::String& parameterID, float newValue) override;

    // Called from JS via the __JUCE__ bridge
    void handleJSMessage(const juce::var& message);

    // Send full parameter state to JS (called on UI load)
    void sendFullState();

private:
    juce::AudioProcessorValueTreeState& apvt;
    juce::WebBrowserComponent& browser;

    // Serialize single param update to JSON
    juce::String paramToJSON(const juce::String& id, float value);

    // Serialize all params to JSON
    juce::String fullStateToJSON();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterBridge)
};
```

**ParameterBridge.cpp — key methods**
```cpp
ParameterBridge::ParameterBridge(juce::AudioProcessorValueTreeState& apvt,
                                  juce::WebBrowserComponent& browser)
    : apvt(apvt), browser(browser)
{
    // Register as listener for every parameter
    for (auto* param : apvt.processor.getParameters())
    {
        if (auto* paramWithID = dynamic_cast<juce::RangedAudioParameter*>(param))
            apvt.addParameterListener(paramWithID->paramID, this);
    }
}

void ParameterBridge::parameterChanged(const juce::String& parameterID, float newValue)
{
    // C++ → JS: parameter changed from host automation, preset load, etc.
    // Must dispatch to message thread since this can fire from audio thread
    juce::MessageManager::callAsync([this, parameterID, newValue]()
    {
        auto json = paramToJSON(parameterID, newValue);
        browser.emitEventIfBrowserIsVisible("parameterUpdate", json);
    });
}

void ParameterBridge::handleJSMessage(const juce::var& message)
{
    // JS → C++: UI knob was turned
    auto paramID = message["id"].toString();
    auto value   = static_cast<float>(message["value"]);

    if (auto* param = apvt.getParameter(paramID))
    {
        // setValueNotifyingHost expects 0–1 normalized range
        param->setValueNotifyingHost(
            param->convertTo0to1(value)
        );
    }
}

void ParameterBridge::sendFullState()
{
    // Called when WebView finishes loading — push all current values
    browser.emitEventIfBrowserIsVisible("fullState", fullStateToJSON());
}
```

### 1.2 — PluginEditor (WebBrowserComponent Setup)

Use the JUCE 7+ `WebBrowserComponent::Options` API. No `evaluateJavascript` string hacking.

```cpp
// PluginEditor.h
class MyPluginEditor : public juce::AudioProcessorEditor
{
public:
    MyPluginEditor(MyPluginProcessor&);
    void resized() override;

private:
    MyPluginProcessor& processor;

    juce::WebBrowserComponent browser {
        juce::WebBrowserComponent::Options{}
            .withNativeIntegrationEnabled()    // Enables window.__JUCE__
            .withEventListener("paramChange",  // JS → C++ channel
                [this](const juce::var& msg) {
                    paramBridge.handleJSMessage(msg);
                })
            .withEventListener("uiReady",      // JS signals it's loaded
                [this](const juce::var&) {
                    paramBridge.sendFullState();
                })
    };

    ParameterBridge paramBridge;
};
```

```cpp
// PluginEditor.cpp
MyPluginEditor::MyPluginEditor(MyPluginProcessor& p)
    : AudioProcessorEditor(p),
      processor(p),
      paramBridge(p.getAPVT(), browser)
{
    setSize(800, 600);
    addAndMakeVisible(browser);

#if JUCE_DEBUG
    // DEV: load from Vite dev server for hot reload
    browser.goToURL("http://localhost:5173");
#else
    // RELEASE: load from embedded BinaryData
    // WebBrowserComponent can load from a ResourceProvider
    // that serves files from BinaryData
    browser.goToURL(juce::WebBrowserComponent::getResourceProviderRoot());
#endif
}

void MyPluginEditor::resized()
{
    browser.setBounds(getLocalBounds());
}
```

### 1.3 — Resource Provider for Release Builds

For release, serve the built React app from `BinaryData`:

```cpp
// In your Options setup, add a resource provider:
.withResourceProvider(
    [](const juce::String& url) -> std::optional<juce::WebBrowserComponent::Resource>
    {
        // Map URL path to BinaryData resource
        auto path = url.fromFirstOccurrenceOf("/", false, false);
        if (path.isEmpty()) path = "index.html";

        // BinaryData names replace . and / with _
        auto resourceName = path.replace("/", "_").replace(".", "_");

        int size = 0;
        if (auto* data = BinaryData::getNamedResource(
                resourceName.toRawUTF8(), size))
        {
            auto mime = url.endsWith(".js")   ? "application/javascript"
                      : url.endsWith(".css")  ? "text/css"
                      : url.endsWith(".html") ? "text/html"
                      :                         "application/octet-stream";

            return juce::WebBrowserComponent::Resource{
                juce::MemoryBlock(data, size), juce::String(mime)
            };
        }
        return std::nullopt;
    })
```

---

## Phase 2: React UI

### 2.1 — Bridge Abstraction (`ui/src/bridge/juce.js`)

This layer decouples React components from the JUCE runtime so you can develop in a browser.

```javascript
// ui/src/bridge/juce.js

const listeners = new Map();  // paramId -> Set<callback>
let parameterState = {};       // current snapshot of all params

// Detect if we're running inside JUCE WebView
const isJUCE = typeof window.__JUCE__ !== 'undefined';

// --- Public API ---

export function setParameter(paramId, value) {
    if (isJUCE) {
        window.__JUCE__.backend.emitEvent("paramChange", {
            id: paramId,
            value: value
        });
    }
    // Update local state immediately for responsive feel
    parameterState[paramId] = value;
    notifyListeners(paramId, value);
}

export function onParameterChange(paramId, callback) {
    if (!listeners.has(paramId)) listeners.set(paramId, new Set());
    listeners.get(paramId).add(callback);

    // Immediately call with current value if we have it
    if (paramId in parameterState) {
        callback(parameterState[paramId]);
    }

    // Return unsubscribe function
    return () => listeners.get(paramId)?.delete(callback);
}

export function getParameterValue(paramId) {
    return parameterState[paramId];
}

// --- Internal: receive from C++ ---

function notifyListeners(paramId, value) {
    listeners.get(paramId)?.forEach(cb => cb(value));
}

if (isJUCE) {
    // C++ pushes individual param updates
    window.__JUCE__.backend.addEventListener("parameterUpdate", (event) => {
        const { id, value } = JSON.parse(event.detail);
        parameterState[id] = value;
        notifyListeners(id, value);
    });

    // C++ pushes full state on load
    window.__JUCE__.backend.addEventListener("fullState", (event) => {
        parameterState = JSON.parse(event.detail);
        for (const [id, value] of Object.entries(parameterState)) {
            notifyListeners(id, value);
        }
    });

    // Signal to C++ that the UI is ready
    window.__JUCE__.backend.emitEvent("uiReady", {});
}
```

### 2.2 — Mock Bridge for Browser Development (`ui/src/bridge/mock.js`)

```javascript
// ui/src/bridge/mock.js
// Used when developing in a standalone browser (no JUCE)

const mockState = {
    reductionFactor: 8.0,
    // Add other params here as you build them
};

export function setParameter(paramId, value) {
    mockState[paramId] = value;
    console.log(`[mock] ${paramId} = ${value}`);
}

export function onParameterChange(paramId, callback) {
    // In mock mode, just call with initial value
    if (paramId in mockState) callback(mockState[paramId]);
    return () => {};
}

export function getParameterValue(paramId) {
    return mockState[paramId];
}
```

### 2.3 — Bridge Entry Point (`ui/src/bridge/index.js`)

```javascript
// Automatically picks the right bridge
const isJUCE = typeof window.__JUCE__ !== 'undefined';

export const { setParameter, onParameterChange, getParameterValue } = isJUCE
    ? await import('./juce.js')
    : await import('./mock.js');
```

### 2.4 — Knob Component

```jsx
// ui/src/components/Knob.jsx
import { useState, useEffect, useRef, useCallback } from 'react';
import { setParameter, onParameterChange } from '../bridge';

export default function Knob({ paramId, label, min = 0, max = 1, step = 0.01 }) {
    const [value, setValue] = useState(min);
    const knobRef = useRef(null);
    const isDragging = useRef(false);
    const startY = useRef(0);
    const startValue = useRef(0);

    // Subscribe to C++ parameter changes (host automation, presets)
    useEffect(() => {
        return onParameterChange(paramId, (newValue) => {
            setValue(newValue);
        });
    }, [paramId]);

    const handleMouseDown = useCallback((e) => {
        isDragging.current = true;
        startY.current = e.clientY;
        startValue.current = value;
        document.addEventListener('mousemove', handleMouseMove);
        document.addEventListener('mouseup', handleMouseUp);
    }, [value]);

    const handleMouseMove = useCallback((e) => {
        if (!isDragging.current) return;
        const delta = (startY.current - e.clientY) * (max - min) / 200;
        const newValue = Math.min(max, Math.max(min, startValue.current + delta));
        const snapped = Math.round(newValue / step) * step;
        setValue(snapped);
        setParameter(paramId, snapped);
    }, [paramId, min, max, step]);

    const handleMouseUp = useCallback(() => {
        isDragging.current = false;
        document.removeEventListener('mousemove', handleMouseMove);
        document.removeEventListener('mouseup', handleMouseUp);
    }, [handleMouseMove]);

    // Rotation: map value range to -135° to +135°
    const rotation = ((value - min) / (max - min)) * 270 - 135;

    return (
        <div className="knob-container">
            <div
                ref={knobRef}
                className="knob"
                onMouseDown={handleMouseDown}
                style={{ transform: `rotate(${rotation}deg)` }}
            >
                <div className="knob-indicator" />
            </div>
            <span className="knob-label">{label}</span>
            <span className="knob-value">{value.toFixed(1)}</span>
        </div>
    );
}
```

### 2.5 — App Root

```jsx
// ui/src/App.jsx
import Knob from './components/Knob';

export default function App() {
    return (
        <div className="plugin-ui">
            <h1>Bitcrusher</h1>
            <Knob
                paramId="reductionFactor"
                label="Reduction"
                min={1.0}
                max={32.0}
                step={0.1}
            />
            {/* Add more knobs here as you expose more APVT params */}
        </div>
    );
}
```

---

## Phase 3: Build System

### 3.1 — Vite Configuration

```javascript
// ui/vite.config.js
import { defineConfig } from 'vite';
import react from '@vitejs/plugin-react';

export default defineConfig({
    plugins: [react()],
    base: './',                          // Relative paths for embedded serving
    build: {
        outDir: 'dist',
        assetsInlineLimit: 100000,       // Inline assets up to ~100KB
                                         // Fewer files = simpler BinaryData
        cssCodeSplit: false,             // Single CSS file
        rollupOptions: {
            output: {
                manualChunks: undefined, // Single JS bundle
                inlineDynamicImports: true,
            }
        }
    },
    server: {
        port: 5173,                      // Dev server for hot reload
        strictPort: true,
    }
});
```

Key difference from original plan: `assetsInlineLimit` is set **high** (not zero) because each separate file in the build output needs its own `BinaryData` entry. Inlining small assets into the JS/CSS bundles means fewer files to manage.

### 3.2 — CMakeLists.txt Integration

```cmake
# After juce_add_plugin(...)

# --- React UI BinaryData ---
# Run: cd ui && npm run build   (before building the C++ project)
file(GLOB_RECURSE UI_DIST_FILES "${CMAKE_SOURCE_DIR}/ui/dist/*")

juce_add_binary_data(PluginUIData
    SOURCES ${UI_DIST_FILES}
)

target_link_libraries(MyPlugin
    PRIVATE
        PluginUIData
        juce::juce_audio_processors
        juce::juce_gui_extra          # Required for WebBrowserComponent
)
```

### 3.3 — Build Workflow

```bash
# Development (two terminals):
cd ui && npm run dev          # Terminal 1: Vite dev server with HMR
# Build and run plugin in DAW  # Terminal 2: JUCE loads from localhost:5173

# Release:
cd ui && npm run build        # Produces ui/dist/
cd .. && cmake --build build  # BinaryData picks up dist/ contents
```

---

## Phase 4: Parameter Sync — Complete Data Flow

### JS → C++ (user turns a knob)

```
1. User drags Knob component
2. Knob calls setParameter("reductionFactor", 12.5)
3. juce.js calls window.__JUCE__.backend.emitEvent("paramChange", {...})
4. PluginEditor's event listener fires
5. ParameterBridge::handleJSMessage() receives the message
6. Calls param->setValueNotifyingHost(normalized)
7. APVT updates → DSP reads new value
8. DAW records automation point
```

### C++ → JS (host automation or preset load)

```
1. DAW sends automation / user loads preset
2. APVT parameter value changes
3. ParameterBridge::parameterChanged() fires
4. Dispatches to message thread
5. Calls browser.emitEventIfBrowserIsVisible("parameterUpdate", json)
6. juce.js event listener receives it
7. Updates parameterState, notifies React subscribers
8. Knob component re-renders with new value
```

### Initial Load

```
1. PluginEditor creates WebBrowserComponent
2. Browser loads React app (dev server or BinaryData)
3. React app initializes, bridge/juce.js runs
4. juce.js emits "uiReady" event
5. PluginEditor's uiReady listener calls paramBridge.sendFullState()
6. juce.js receives "fullState", populates parameterState
7. All subscribed Knob components render with correct initial values
```

---

## Phase 5: State Persistence (Presets / DAW Save)

No extra work needed here if you do it right. Because the APVT is the single source of truth:

- `getStateInformation()` / `setStateInformation()` already serialize the APVT to the DAW session
- When `setStateInformation()` restores state, each parameter change fires `parameterChanged()` on the bridge → React UI updates automatically
- Preset load follows the same path — update APVT params, bridge propagates to UI

The React UI is **stateless with respect to persistence**. It only reflects what the APVT tells it.

---

## Dev Workflow Summary

| Mode      | React Source     | How It Works                                             |
|-----------|------------------|----------------------------------------------------------|
| Browser   | `npm run dev`    | Opens in Chrome, uses mock bridge, no JUCE needed        |
| Plugin Dev| `npm run dev`    | JUCE WebView loads `localhost:5173`, hot reload works     |
| Release   | `npm run build`  | JUCE serves from BinaryData, single binary distribution  |

---

## Implementation Order

1. **ParameterBridge class** — get C++ ↔ JS messaging working with a single param
2. **PluginEditor with WebBrowserComponent** — load a static HTML page, verify the bridge connects
3. **React project scaffold** — Vite + React, bridge abstraction with mock
4. **Single Knob → reductionFactor** — end-to-end: turn knob in UI, hear bitcrusher change
5. **Reverse direction** — move a DAW automation fader, watch the React knob move
6. **Full state sync** — initial load, preset save/restore
7. **CMake BinaryData integration** — release build pipeline
8. **Additional parameters** — once the pipeline works, adding params is trivial