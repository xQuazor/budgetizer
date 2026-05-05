// JUCE bridge — only active when running inside a JUCE WebBrowserComponent

const listeners = new Map();  // paramId -> Set<callback>
let parameterState = {};
const licenseListeners = new Set();
let lastLicenseStatus = null;
const fullStateListeners = new Set();
let lastFullState = null;

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

export function submitLicense(key) {
    if (isJUCE) {
        window.__JUCE__.backend.emitEvent("submitLicense", { key });
    }
}

export function onLicenseStatus(callback) {
    licenseListeners.add(callback);
    if (lastLicenseStatus !== null) callback(lastLicenseStatus);
    return () => licenseListeners.delete(callback);
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
        lastFullState = event;
        for (const [id, value] of Object.entries(event)) {
            notifyListeners(id, value);
        }
        fullStateListeners.forEach(cb => cb(event));
    });

    // C++ → JS: license validation result
    window.__JUCE__.backend.addEventListener("licenseStatus", (event) => {
        lastLicenseStatus = event;
        licenseListeners.forEach(cb => cb(event));
    });

    // Signal to C++ that the UI is ready
    window.__JUCE__.backend.emitEvent("uiReady", {});
}

export function onFullState(callback) {
    fullStateListeners.add(callback);
    if (lastFullState !== null) callback(lastFullState);
    return () => fullStateListeners.delete(callback);
}
