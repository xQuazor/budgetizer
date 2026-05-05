// Automatically picks the real JUCE bridge or the mock for browser dev

import * as juceBridge from './juce.js';
import * as mockBridge from './mock.js';

// juce.js guards all window.__JUCE__ access internally, so both can be imported safely
const bridge = typeof window.__JUCE__ !== 'undefined' ? juceBridge : mockBridge;

export const setParameter       = bridge.setParameter;
export const onParameterChange  = bridge.onParameterChange;
export const onFullState        = bridge.onFullState;

export const submitLicense       = bridge.submitLicense;

export const onLicenseStatus = bridge.onLicenseStatus;
export const getParameterValue  = bridge.getParameterValue;
