import { useState } from "react"

export default function Toolbar({ presets, onPresetLoad }) {
  const [selected, setSelected] = useState("")

  const handleChange = (e) => {
    const idx = e.target.value
    if (idx === "") return
    setSelected(idx)
    onPresetLoad(presets[parseInt(idx)])
  }

  return (
    <div className="flex items-center justify-end w-full px-3.5 h-8 bg-gray-200 shadow-md">
      <div className="flex items-center justify-center gap-2">
        <span className="black text-[8px] tracking-widest uppercase select-none">
          Preset
        </span>

        <div className="relative">
          <select
            value={selected}
            onChange={handleChange}
            className={[
              "appearance-none bg-gray-800 border border-gray-600 rounded",
              "text-[10px] py-0.5 pl-2.5 pr-6 cursor-pointer outline-none min-w-32.5",
              selected === "" ? "text-white/25" : "text-white/80",
            ].join(" ")}
          >
            <option value="" disabled>— select —</option>
            {presets.map((p, i) => (
              <option key={i} value={i} className="bg-gray-800 text-gray-300">
                {p.name}
              </option>
            ))}
          </select>

          <svg
            viewBox="0 0 10 6"
            className="absolute right-2 top-1/2 -translate-y-1/2 w-2 pointer-events-none fill-none stroke-white/30 stroke-[1]"
          >
            <path d="M1 1l4 4 4-4" strokeLinecap="round" strokeLinejoin="round" />
          </svg>
        </div>
      </div>
    </div>
  )
}
