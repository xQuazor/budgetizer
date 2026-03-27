import { useState } from 'react';
import Knob from "./components/Knob.jsx";
import ModeButton from "./components/ModeButton.jsx";
import SpeakerGrill from "./components/Speaker.jsx";

export default function App() {
  const [smoothActive, setSmoothActive] = useState(false);

  return (
    <div className={"w-full h-full block"}>

        <div className={"flex flex-row gap-8 bg-blue-500 p-6"}>

            <div className={"relative flex flex-col items-center justify-center w-fit gap-8 rounded-xl px-4 py-8"} key={"SettingContainer"}>
                <div style={{
                    position: 'absolute',
                    inset: 0,
                    borderRadius: 'inherit',
                    border: '1px solid transparent',
                    background: 'radial-gradient(circle, #000000 0%, #666666 100%) border-box',
                    WebkitMask: 'linear-gradient(#fff 0 0) padding-box, linear-gradient(#fff 0 0)',
                    WebkitMaskComposite: 'destination-out',
                    maskComposite: 'exclude',
                    mixBlendMode: 'screen',
                    pointerEvents: 'none',
                }} />
                <ModeButton  label="Smooth" active={smoothActive} onClick={() => setSmoothActive(v => !v)} />
                <div className={"flex flex-col items-center justify-center gap-4 w-fit"}>
                    <Knob compact={false} paramId="bitDepth" label="Depth" min={1} max={16} step={1} />
                    <Knob compact={false} paramId="rate" label="Rate" min={1} max={16} step={1} />
                </div>
            </div>

            <div className={"relative flex flex-col items-center justify-center w-fit gap-8 rounded-xl px-4 py-8"} key={"SettingContainer"}>
                <div style={{
                    position: 'absolute',
                    inset: 0,
                    borderRadius: 'inherit',
                    border: '1px solid transparent',
                    background: 'radial-gradient(circle, #000000 0%, #666666 100%) border-box',
                    WebkitMask: 'linear-gradient(#fff 0 0) padding-box, linear-gradient(#fff 0 0)',
                    WebkitMaskComposite: 'destination-out',
                    maskComposite: 'exclude',
                    mixBlendMode: 'screen',
                    pointerEvents: 'none',
                }} />
                <ModeButton  label="Smooth" active={smoothActive} onClick={() => setSmoothActive(v => !v)} />
                <div className={"flex flex-col items-center justify-center gap-4 w-fit"}>
                    <Knob compact={false} paramId="bitDepth" label="Depth" min={1} max={16} step={1} />
                    <Knob compact={false} paramId="rate" label="Rate" min={1} max={16} step={1} />
                </div>
            </div>

            <SpeakerGrill/>

            <div className={"relative flex flex-col items-center justify-center w-fit gap-8 rounded-xl px-4 py-8"} key={"SettingContainer"}>
                <div style={{
                    position: 'absolute',
                    inset: 0,
                    borderRadius: 'inherit',
                    border: '1px solid transparent',
                    background: 'radial-gradient(circle, #000000 0%, #666666 100%) border-box',
                    WebkitMask: 'linear-gradient(#fff 0 0) padding-box, linear-gradient(#fff 0 0)',
                    WebkitMaskComposite: 'destination-out',
                    maskComposite: 'exclude',
                    mixBlendMode: 'screen',
                    pointerEvents: 'none',
                }} />
                <ModeButton  label="Smooth" active={smoothActive} onClick={() => setSmoothActive(v => !v)} />
                <div className={"flex flex-col items-center justify-center gap-4 w-fit"}>
                    <Knob compact={false} paramId="bitDepth" label="Depth" min={1} max={16} step={1} />
                    <Knob compact={false} paramId="rate" label="Rate" min={1} max={16} step={1} />
                </div>
            </div>

        </div>
    </div>
  );
}
