// JUCE bridge — only active when running inside a JUCE WebBrowserComponent

const listeners = new Map();  // paramId -> Set<callback>
let parameterState = {};

const isJUCE = typeof window.__JUCE__ !== 'undefined';

export function setParameter(paramId, value) {
    if (isJUCE) {
        window.__JUCE__.backend.emitEvent("paramChange", { id: paramId, value });
    }
    parameterState[paramId] = value;
    notifyListeners(paramId, value);
}

export function onParameterChange(paramId, callback) {
    if (!listeners.has(paramId)) listeners.set(paramId, new Set());
    listeners.get(paramId).add(callback);

    if (paramId in parameterState) callback(parameterState[paramId]);

    return () => listeners.get(paramId)?.delete(callback);
}

export function getParameterValue(paramId) {
    return parameterState[paramId];
}

function notifyListeners(paramId, value) {
    listeners.get(paramId)?.forEach(cb => cb(value));
}

if (isJUCE) {
    // C++ → JS: single parameter update (host automation, preset load)
    window.__JUCE__.backend.addEventListener("parameterUpdate", (event) => {
        const { id, value } = event;
        parameterState[id] = value;
        notifyListeners(id, value);
    });

    // C++ → JS: full state on initial load
    window.__JUCE__.backend.addEventListener("fullState", (event) => {
        Object.assign(parameterState, event);
        for (const [id, value] of Object.entries(event)) {
            notifyListeners(id, value);
        }
    });

    // Signal to C++ that the UI is ready
    window.__JUCE__.backend.emitEvent("uiReady", {});
}
