import { useState, useEffect, useRef, useCallback } from 'react';
import { setParameter, onParameterChange } from '../bridge';

export default function Knob({ paramId, label, min = 0, max = 1, step = 0.01 }) {
    const [value, setValue] = useState(min);
    const isDragging  = useRef(false);
    const startY      = useRef(0);
    const startValue  = useRef(0);

    // Subscribe to C++ parameter changes (host automation, preset load)
    useEffect(() => {
        return onParameterChange(paramId, (newValue) => setValue(newValue));
    }, [paramId]);

    const handleMouseMove = useCallback((e) => {
        if (!isDragging.current) return;
        const delta    = (startY.current - e.clientY) * (max - min) / 200;
        const clamped  = Math.min(max, Math.max(min, startValue.current + delta));
        const snapped  = Math.round(clamped / step) * step;
        setValue(snapped);
        setParameter(paramId, snapped);
    }, [paramId, min, max, step]);

    const handleMouseUp = useCallback(() => {
        isDragging.current = false;
        document.removeEventListener('mousemove', handleMouseMove);
        document.removeEventListener('mouseup', handleMouseUp);
    }, [handleMouseMove]);

    const handleMouseDown = useCallback((e) => {
        isDragging.current  = true;
        startY.current      = e.clientY;
        startValue.current  = value;
        document.addEventListener('mousemove', handleMouseMove);
        document.addEventListener('mouseup',   handleMouseUp);
    }, [value, handleMouseMove, handleMouseUp]);

    // Map value range to -135° … +135°
    const rotation = ((value - min) / (max - min)) * 270 - 135;

    return (
        <div className="knob-container">
            <div
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
