import { useState, useEffect, useRef, useCallback } from "react";
import { setParameter, onParameterChange } from "../bridge";
const imgEllipse34 =
  "https://www.figma.com/api/mcp/asset/04bcb213-706a-4c16-a8b1-eab4f087e7f9";

export default function Knob({
  paramId,
  label,
  unit = "",
  min = 0,
  max = 1,
  step = 0.01,
  scale = 1,
  value: externalValue,
  setValue: externalSetValue,
}) {
  const [internalValue, setInternalValue] = useState(min);
  const value = externalValue !== undefined ? externalValue : internalValue;
  const setValue =
    externalSetValue !== undefined ? externalSetValue : setInternalValue;
  const isDragging = useRef(false);
  const startY = useRef(0);
  const startValue = useRef(0);

  const handleMouseMove = useCallback(
    (e) => {
      if (!isDragging.current) return;
      const delta = ((startY.current - e.clientY) * (max - min)) / 200;
      const clamped = Math.min(max, Math.max(min, startValue.current + delta));
      const snapped = Math.round(clamped / step) * step;
      setValue(snapped);
      setParameter(paramId, snapped);
    },
    [paramId, min, max, step],
  );

  const handleMouseUp = useCallback(() => {
    isDragging.current = false;
    document.removeEventListener("mousemove", handleMouseMove);
    document.removeEventListener("mouseup", handleMouseUp);
  }, [handleMouseMove]);

  const handleMouseDown = useCallback(
    (e) => {
      e.preventDefault();
      isDragging.current = true;
      startY.current = e.clientY;
      startValue.current = value;
      document.addEventListener("mousemove", handleMouseMove);
      document.addEventListener("mouseup", handleMouseUp);
    },
    [value, handleMouseMove, handleMouseUp],
  );

  const rotation = ((value - min) / (max - min)) * 180 - 90;
  const displayValue = step >= 1 ? Math.round(value) : value.toFixed(2);

  return (
    <div
      className="relative h-18 w-14.5 cursor-ns-resize select-none block"
      style={{ zoom: scale }}
      onMouseDown={handleMouseDown}
    >
      {/* Dial body — static button face + shading */}
      <div
        className="absolute rounded-full drop-shadow-[-2px_3px_7px_rgba(0,0,0,0.25)] w-8 h-8"
        style={{
          inset: "24.19% 22.81% 25.81% 21.05%",
          outline: `1px solid rgba(0, 0, 0, 0.7)`,
        }}
      >
        <div
          className={"absolute rounded-full bg-[#70A0C3] w-8 h-8"}
          style={{ boxShadow: "inset 6px 3px 4px 0px rgba(0,0,0,0.45)" }}
        />
        <div
          id={"this"}
          className={"absolute rounded-full w-8 h-8"}
          style={{
            background:
              "radial-gradient(50% 50% at 50% 50%, rgba(0, 0, 0, 0.40) 0%, rgba(0, 0, 0, 0.00) 100%)",
            mixBlendMode: "overlay",
          }}
        />

        <div
          id="here"
          className={
            "-right-0.25 -bottom-0.25 overflow-hidden rounded-full absolute bg-[#70A0C3] w-7.75 h-7.75"
          }
          style={{
            rotate: `${rotation}deg`,
          }}
        >
          <div
            className={
              "absolute w-3 h-0.75 bg-white rotate-90 translate-1/2 right-1/2"
            }
          />
        </div>
        {/* White highlight — static layer on top */}
        <div
          className="-right-0.25 -bottom-0.25 overflow-hidden rounded-full absolute w-7.75 h-7.75 pointer-events-none"
          style={{ boxShadow: "inset -2px 3px 4px 0px rgba(255,255,255,0.3)" }}
        />
      </div>

      {/* Arc range indicator — top half circle */}
      <div
        className="absolute pointer-events-none"
        style={{ inset: "19.35% 15.79% 14.52% 12.28%" }}
      >
        <div
          className="absolute top-0 left-0 right-0"
          style={{
            height: "50%",
            borderRadius: "100px 100px 0 0",
            border: "1px solid rgba(255,255,255,0.6)",
            borderBottom: "none",
            background: "transparent",
          }}
        />
      </div>

      {/* Text overlay — value / range / label */}
      <div className="absolute inset-0 flex flex-col gap-[16px] items-start font-['Inter',sans-serif] font-normal text-[8px] text-[#ddd] leading-normal pointer-events-none">
        <p className="w-full text-center shrink-0">
          {displayValue}
          {unit}
        </p>
        <div className="flex items-center justify-between w-[57px] shrink-0 whitespace-nowrap">
          <span>-</span>
          <span>+</span>
        </div>
        <p className="w-full text-center shrink-0">{label}</p>
      </div>
    </div>
  );
}
