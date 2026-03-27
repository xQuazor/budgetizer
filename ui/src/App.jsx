import { useState, useRef, useCallback } from 'react';
import SpeakerLegs from "./components/SpeakerLegs.jsx";
import Antenna from "./components/Antenna.jsx";
import {Body} from "./components/Body.jsx";
import Knob from "./components/Knob.jsx";
import SpeakerGrill from "./components/Speaker.jsx";
import ModeButton from "./components/ModeButton.jsx";

const smoothIcon = (
    <div className="absolute inset-0">
        <img alt="" className="absolute block max-w-none" style={{ left: '-3.5px', top: '0px',  width: '21px', height: '21px' }} src={imgSmoothShading} />
        <img alt="" className="absolute block max-w-none" style={{ left: '3.5px',  top: '4px',  width: '16px', height: '16px' }} src={imgSmoothButtonSide} />
        <img alt="" className="absolute block max-w-none" style={{ left: '5.5px',  top: '6px',  width: '14px', height: '14px' }} src={imgSmoothButtonTop} />
    </div>
);


export default function App() {
  const [smoothActive, setSmoothActive] = useState(false);
  const [radioActive,  setRadioActive]  = useState(true);
  const [syncActive,   setSyncActive]   = useState(false);

  return (
    <div className={"w-full h-full block"}>
        <div className={"flex flex-col gap-12 w-fit"}>
            <ModeButton  label="Smooth" active={smoothActive} onClick={() => setSmoothActive(v => !v)} />
            {/*<Knob compact={false} paramId="bitDepth" label="Depth" min={1} max={16} step={1} />*/}
            {/*<Knob compact={false} paramId="rate" label="Rate" min={1} max={16} step={1} />*/}
        </div>

      {/*<SpeakerGrill />*/}

      {/*<SpeakerLegs/>*/}
      {/*<Antenna/>*/}
      {/*<Body/>*/}
      {/* UI overlay */}
      {/*<div className="absolute left-[63px] top-[106px] w-[521px] h-[305px] p-6 flex flex-col justify-between">*/}

      {/*  /!* Branding bar *!/*/}
      {/*  <div className="flex items-end justify-between w-full shadow-[0_-1px_3.6px_rgba(0,0,0,0.15)] pb-1">*/}
      {/*    <div className="flex flex-col gap-0.5">*/}
      {/*      <span className="font-['Fira_Code',monospace] font-semibold text-[18px] text-[#d8d8d8] leading-none tracking-[0.02em]">BUDGETIZER</span>*/}
      {/*      <span className="font-['Fira_Code',monospace] font-light text-[8px] text-[#c9c9c9] leading-none">Audio Degradation Unit</span>*/}
      {/*    </div>*/}
      {/*    <span className="font-['Fira_Code',monospace] font-normal text-[10px] text-[#d8d8d8] leading-none">daveys audio</span>*/}
      {/*  </div>*/}

      {/*  /!* Controls + speaker row *!/*/}
      {/*  <div className="flex items-stretch justify-between w-full flex-1 mt-4">*/}

      {/*    /!* Left panel: Smooth / Rate / Bits *!/*/}
      {/*    <div className="flex flex-col items-center justify-between border border-black rounded-[8px] px-2 py-4 h-[212px]">*/}
      {/*      <ModeButton icon={IMG.iconSmooth} label="Smooth" active={smoothActive} onClick={() => setSmoothActive(v => !v)} />*/}
      {/*      <div className="flex flex-col items-center gap-[11px]">*/}
      {/*        <Knob compact paramId="reductionFactor" label="Rate" min={1} max={32} step={1} />*/}
      {/*        <Knob compact paramId="bitDepth" label="Bits" min={1} max={16} step={1} />*/}
      {/*      </div>*/}
      {/*    </div>*/}

      {/*    /!* Middle panel: Radio / Burst & Drift / Depth *!/*/}
      {/*    <div className="flex flex-col items-center justify-between border border-black rounded-[8px] px-2 py-4 h-[212px]">*/}
      {/*      <ModeButton icon={IMG.iconRadio} label="Radio" active={radioActive} onClick={() => setRadioActive(v => !v)} />*/}
      {/*      <div className="flex flex-col items-center gap-[11px]">*/}
      {/*        <Knob compact paramId="cutoff" label="Burst & Drift" min={0} max={20000} step={100} />*/}
      {/*        <LocalKnob label="Depth" min={0} max={1} step={0.01} defaultValue={0.25} />*/}
      {/*      </div>*/}
      {/*    </div>*/}


      {/*    /!* Right panel: Sync / Mix *!/*/}
      {/*    <div className="flex flex-col items-center justify-between border border-black rounded-[8px] p-4 h-[212px]">*/}
      {/*      <ModeButton icon={IMG.iconSmooth} label="Sync" active={syncActive} onClick={() => setSyncActive(v => !v)} />*/}
      {/*      <div className="flex flex-col items-center gap-[11px]">*/}
      {/*        <LocalKnob label="Mix" min={0} max={1} step={0.01} defaultValue={0.2} />*/}
      {/*      </div>*/}
      {/*    </div>*/}

      {/*  </div>*/}
      {/*</div>*/}
    </div>
  );
}
