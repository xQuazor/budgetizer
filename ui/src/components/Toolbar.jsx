import { useState } from "react";

export default function Toolbar({ presets, onPresetLoad }) {
  const [selected, setSelected] = useState("");

  const handleChange = (e) => {
    const idx = e.target.value;
    if (idx === "") return;
    setSelected(idx);
    onPresetLoad(presets[parseInt(idx)]);
  };

  return (
    <div style={{
      display: "flex",
      alignItems: "center",
      justifyContent: "flex-end",
      padding: "0 14px",
      height: 36,
      background: "#111",
      borderBottom: "1px solid #2a2a2a",
    }}>
      <div style={{ display: "flex", alignItems: "center", gap: 8 }}>
        <span style={{
          color: "rgba(255,255,255,0.3)",
          fontSize: 10,
          letterSpacing: "0.08em",
          textTransform: "uppercase",
          userSelect: "none",
        }}>
          Preset
        </span>

        <div style={{ position: "relative" }}>
          <select
            value={selected}
            onChange={handleChange}
            style={{
              appearance: "none",
              background: "#1c1c1c",
              border: "1px solid #333",
              borderRadius: 5,
              color: selected === "" ? "rgba(255,255,255,0.25)" : "rgba(255,255,255,0.8)",
              fontSize: 11,
              padding: "3px 26px 3px 10px",
              cursor: "pointer",
              outline: "none",
              minWidth: 130,
            }}
          >
            <option value="" disabled>— select —</option>
            {presets.map((p, i) => (
              <option key={i} value={i} style={{ background: "#1c1c1c", color: "#ccc" }}>
                {p.name}
              </option>
            ))}
          </select>

          <svg
            viewBox="0 0 10 6"
            style={{
              position: "absolute",
              right: 8,
              top: "50%",
              transform: "translateY(-50%)",
              width: 8,
              pointerEvents: "none",
              fill: "none",
              stroke: "rgba(255,255,255,0.3)",
              strokeWidth: 1.5,
            }}
          >
            <path d="M1 1l4 4 4-4" strokeLinecap="round" strokeLinejoin="round" />
          </svg>
        </div>
      </div>
    </div>
  );
}
