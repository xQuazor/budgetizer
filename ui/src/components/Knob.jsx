import { useState, useEffect, useRef, useCallback } from 'react';
import { setParameter, onParameterChange } from '../bridge';

const imgButtonSide = "https://www.figma.com/api/mcp/asset/e7810ea3-ab0b-4250-a658-acb1745fce54";
const imgShading    = "https://www.figma.com/api/mcp/asset/dd344ac6-3167-413a-aba0-c895761ce97c";
const imgFrame66    = "https://www.figma.com/api/mcp/asset/43c9d8b0-fb0a-45bf-856f-c98ccf40b548";
const imgEllipse34  = "https://www.figma.com/api/mcp/asset/04bcb213-706a-4c16-a8b1-eab4f087e7f9";

export default function Knob({ paramId, label, min = 0, max = 1, step = 0.01 }) {
    const [value, setValue] = useState(min);
    const isDragging  = useRef(false);
    const startY      = useRef(0);
    const startValue  = useRef(0);

    useEffect(() => {
        return onParameterChange(paramId, (newValue) => setValue(newValue));
    }, [paramId]);

    const handleMouseMove = useCallback((e) => {
        if (!isDragging.current) return;
        const delta   = (startY.current - e.clientY) * (max - min) / 200;
        const clamped = Math.min(max, Math.max(min, startValue.current + delta));
        const snapped = Math.round(clamped / step) * step;
        setValue(snapped);
        setParameter(paramId, snapped);
    }, [paramId, min, max, step]);

    const handleMouseUp = useCallback(() => {
        isDragging.current = false;
        document.removeEventListener('mousemove', handleMouseMove);
        document.removeEventListener('mouseup', handleMouseUp);
    }, [handleMouseMove]);

    const handleMouseDown = useCallback((e) => {
        isDragging.current = true;
        startY.current     = e.clientY;
        startValue.current = value;
        document.addEventListener('mousemove', handleMouseMove);
        document.addEventListener('mouseup',   handleMouseUp);
    }, [value, handleMouseMove, handleMouseUp]);

    const rotation = ((value - min) / (max - min)) * 270 - 135;
    const displayValue = step >= 1 ? Math.round(value) : value.toFixed(2);

    return (
        <div
            className="relative h-[62px] w-[57px] cursor-ns-resize select-none"
            onMouseDown={handleMouseDown}
        >
            {/* Dial body — static button face + shading */}
            <div className="absolute overflow-hidden rounded-[100px] shadow-[-3px_4px_6.9px_0px_rgba(0,0,0,0.25)]"
                 style={{ inset: '24.19% 22.81% 25.81% 21.05%' }}>
                <img alt="" className="absolute block h-[31px] w-[32px] left-0 top-0 max-w-none" src={imgButtonSide} />
                <img alt="" className="absolute block size-[35px] left-[-12px] top-[-2px] max-w-none" src={imgShading} />
                {/* Rotating face / indicator */}
                <img
                    alt=""
                    className="absolute block size-[29px] left-[3px] top-[2px] max-w-none"
                    src={imgFrame66}
                    style={{ transformOrigin: 'center center', transform: `rotate(${rotation}deg)` }}
                />
            </div>

            {/* Arc range indicator — static */}
            <div className="absolute" style={{ inset: '19.35% 15.79% 14.52% 12.28%' }}>
                <div className="absolute top-0 left-0 right-0 bottom-1/2">
                    <img alt="" className="block max-w-none size-full" src={imgEllipse34} />
                </div>
            </div>

            {/* Text overlay — value / range / label */}
            <div className="absolute inset-0 flex flex-col gap-[16px] items-start font-['Inter',sans-serif] font-normal text-[8px] text-[#ddd] leading-normal pointer-events-none">
                <p className="w-full text-center shrink-0">{displayValue}</p>
                <div className="flex items-center justify-between w-[57px] shrink-0 whitespace-nowrap">
                    <span>-</span>
                    <span>+</span>
                </div>
                <p className="w-full text-center shrink-0">{label}</p>
            </div>
        </div>
    );
}
