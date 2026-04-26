// Mock bridge for browser development (no JUCE runtime)

const listeners = new Map();
const licenseListeners = new Set();
const mockState = {
    bitDepth: 8,
    reductionFactor: 4,
    cutoff: 5000,
    dithering: false,
    linearInterp: true,
    polyBlep: false,
};

export function setParameter(paramId, value) {
    mockState[paramId] = value;
    console.log(`[mock] ${paramId} = ${value}`);
    listeners.get(paramId)?.forEach(cb => cb(value));
}

export function onParameterChange(paramId, callback) {
    if (!listeners.has(paramId)) listeners.set(paramId, new Set());
    listeners.get(paramId).add(callback);

    if (paramId in mockState) callback(mockState[paramId]);

    return () => listeners.get(paramId)?.delete(callback);
}

export function getParameterValue(paramId) {
    return mockState[paramId];
}

export function submitLicense(_key) {
    setTimeout(() => {
        licenseListeners.forEach(cb => cb({ valid: false }));
    }, 400);
}

export function onLicenseStatus(callback) {
    licenseListeners.add(callback);
    return () => licenseListeners.delete(callback);
}
