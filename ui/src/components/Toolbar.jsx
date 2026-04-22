import { useState } from "react";
import "./toolbar.css";
import lcdTexture from "../assets/lcd-texture.png";
import ModeButton from "@/components/ModeButton.jsx";
export default function Toolbar({ presets, onPresetLoad, bypass, onBypassToggle }) {
  const [selected, setSelected] = useState("");

  const handleChange = (e) => {
    const idx = e.target.value;
    if (idx === "") return;
    setSelected(idx);
    onPresetLoad(presets[parseInt(idx)]);
  };

  const navigate = (dir) => {
    if (!presets.length) return;
    const current = selected === "" ? -1 : parseInt(selected);
    const next = Math.max(0, Math.min(presets.length - 1, current + dir));
    setSelected(String(next));
    onPresetLoad(presets[next]);
  };

  return (
    <div
      className={`flex items-center justify-between w-full px-3.5 h-8.25 bg-[#73A2C5] metalOutline`}
    >
      <div className="relative w-64 h-5.5 overflow-hidden rounded-xs">
        <div className={"flex flex-row gap-2 absolute right-2 top-0.5 z-20 items-center justify-center"}>
          <button onClick={() => navigate(-1)} className={"text-[#87ACC8] glowText text-xs scale-y-150 cursor-pointer"}>&lt;</button>
          <button onClick={() => navigate(1)} className={"text-[#87ACC8] glowText text-xs scale-y-150 cursor-pointer"}>&gt;</button>
        </div>
        <img
          src={lcdTexture}
          alt={"lcd text"}
          className={
            "z-10 absolute w-full h-full opacity-20 mix-blend-multiply pointer-events-none"
          }
        />

        <select
          value={selected}
          onChange={handleChange}
          className={[
            "absolute z-0 appearance-none bg-[#2F3767] rounded glowText",
            "text-[10px] py-0.5 pl-2.5 pr-6 cursor-pointer outline-none w-full h-full",
            selected === "" ? "text-white/25" : "text-[#87ACC8]",
          ].join(" ")}
        >
          <option value="" disabled className={"text-[#87ACC8] glowText"}>
            — PRESETS —
          </option>
          {presets.map((p, i) => (
            <option
              key={i}
              value={i}
              className="bg-gray-800 text-[#87ACC8] glowText"
            >
              {p.name}
            </option>
          ))}
        </select>
      </div>
      <div className={"flex flex-row gap-2"}>
        <div className={"relative w-12 h-5 rounded overflow-hidden"}>
          <p
            className={"glowText text-[9px] absolute z-20 top-0.75 right-1.5"}
            style={{
              color: bypass ? "#3a4f62" : "#87ACC8",
              transition: "color 0.25s ease",
            }}
          >
            BYPASS
          </p>
          <img
              src={lcdTexture}
              alt={"lcd text"}
              className={
                "z-10 absolute w-full h-full opacity-20 mix-blend-multiply pointer-events-none"
              }
          />
          <div
            className={"rounded w-14 h-5.5 absolute z-0"}
            style={{
              backgroundColor: bypass ? "#5A6FA8" : "#2F3767",
              transition: "background-color 0.25s ease",
            }}
          />
        </div>
        <ModeButton label="" value={bypass} onClick={onBypassToggle} />
      </div>
    </div>
  );
}
