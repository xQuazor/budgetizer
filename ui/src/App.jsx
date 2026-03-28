import { useState, useEffect, useRef } from "react";
import { setParameter } from "./bridge";
import Knob from "./components/Knob.jsx";
import ModeButton from "./components/ModeButton.jsx";
import SpeakerGrill from "./components/SpeakerGrill.jsx";
import BrandingText from "./components/BrandingText.jsx";
import Antenna from "./components/Antenna.jsx";
import {Body} from "./components/Body.jsx";
import {Speaker} from "./components/Speaker.jsx";
import SpeakerLeg from "./components/SpeakerLeg.jsx";

export default function App() {
  const containerRef = useRef(null);
  const [smooth, setSmooth] = useState(false);
  const [radio, setRadio] = useState(false);
  const [sync, setSync] = useState(false);

  const [bitDepth, setBitDepth] = useState(8);
  const [rate, setRate] = useState(5);
  const [burstDrift, setBurstDrift] = useState(20);
  const [depth, setDepth] = useState(25);
  const [mix, setMix] = useState(50);

  useEffect(() => { setParameter("masterMix",     mix);     }, [mix]);
  useEffect(() => { setParameter("sampleReductionRate",     rate);     }, [rate]);
  useEffect(() => { setParameter("bitDepth",     bitDepth);     }, [bitDepth]);
  useEffect(() => { setParameter("radio",     radio);     }, [radio]);
  useEffect(() => { setParameter("smooth",     smooth);     }, [smooth]);

  useEffect(() => {
    if (!containerRef.current || typeof window.__JUCE__ === 'undefined') return;
    requestAnimationFrame(() => {
      const rect = containerRef.current.getBoundingClientRect();
      window.__JUCE__.backend.emitEvent("resize", {
        width:  Math.round(rect.width),
        height: Math.round(rect.height),
      });
    });
  }, []);

  const settingContainerStyles =
    "relative flex flex-col items-center justify-between w-fit gap-4 rounded-xl px-3 py-4";
  const knobContainerStyles =
    "flex flex-col items-center justify-center gap-2 w-fit";

  return (
    <div key={"ApplicationContainer"} ref={containerRef} className={"w-fit h-fit"}>
      <Speaker className={"relative py-2 flex flex-col gap w-fit"}>
        <Antenna className={"-top-1"}/>
        <Body>
          <div className={"flex flex-col gap-4 p-6 w-fit"}>
            <BrandingText />
            <div className={"flex flex-row gap-4"}>
              <div className={settingContainerStyles} key={"SettingContainer"}>
                <div
                  style={{
                    position: "absolute",
                    inset: 0,
                    borderRadius: "inherit",
                    border: "1px solid transparent",
                    background:
                      "radial-gradient(circle, #000000 0%, #666666 100%) border-box",
                    WebkitMask:
                      "linear-gradient(#fff 0 0) padding-box, linear-gradient(#fff 0 0)",
                    WebkitMaskComposite: "destination-out",
                    maskComposite: "exclude",
                    mixBlendMode: "screen",
                    pointerEvents: "none",
                  }}
                />
                <ModeButton
                  label="Smooth"
                  value={smooth}
                  onClick={() => setSmooth((v) => !v)}
                />
                <div className={knobContainerStyles}>
                  <Knob
                    label="Rate"
                    unit={"kHz"}
                    min={1}
                    max={22}
                    step={1}
                    value={rate}
                    setValue={setRate}
                  />
                  <Knob
                    label="Bits"
                    min={1}
                    max={24}
                    step={1}
                    value={bitDepth}
                    setValue={setBitDepth}
                  />
                </div>
              </div>

              <div className={settingContainerStyles} key={"SettingContainer2"}>
                <div
                  style={{
                    position: "absolute",
                    inset: 0,
                    borderRadius: "inherit",
                    border: "1px solid transparent",
                    background:
                      "radial-gradient(circle, #000000 0%, #666666 100%) border-box",
                    WebkitMask:
                      "linear-gradient(#fff 0 0) padding-box, linear-gradient(#fff 0 0)",
                    WebkitMaskComposite: "destination-out",
                    maskComposite: "exclude",
                    mixBlendMode: "screen",
                    pointerEvents: "none",
                  }}
                />
                <ModeButton
                  label="Radio"
                  value={radio}
                  onClick={() => setRadio((v) => !v)}
                />
                <div className={knobContainerStyles}>
                  <Knob
                    label="Burst & Drift"
                    unit={"%"}
                    min={1}
                    max={100}
                    step={1}
                    value={burstDrift}
                    setValue={setBurstDrift}
                  />
                  <Knob
                    label="Depth"
                    unit={"%"}
                    min={1}
                    max={100}
                    step={1}
                    value={depth}
                    setValue={setDepth}
                  />
                </div>
              </div>

              <SpeakerGrill />

              <div className={settingContainerStyles} key={"SettingContainer3"}>
                <div
                  style={{
                    position: "absolute",
                    inset: 0,
                    borderRadius: "inherit",
                    border: "1px solid transparent",
                    background:
                      "radial-gradient(circle, #000000 0%, #666666 100%) border-box",
                    WebkitMask:
                      "linear-gradient(#fff 0 0) padding-box, linear-gradient(#fff 0 0)",
                    WebkitMaskComposite: "destination-out",
                    maskComposite: "exclude",
                    mixBlendMode: "screen",
                    pointerEvents: "none",
                  }}
                />
                <ModeButton
                  label="Sync"
                  value={sync}
                  onClick={() => setSync((v) => !v)}
                />
                <div className={knobContainerStyles}>
                  <Knob
                    label="Mix"
                    unit={"%"}
                    min={0.01}
                    max={1.0}
                    step={0.01}
                    value={mix}
                    setValue={setMix}
                  />
                </div>
              </div>
            </div>
          </div>
        </Body>
        {/*<div className={"flex flex-row w-full px-16 justify-between absolute bottom-1"}>*/}
        {/*    <SpeakerLeg/>*/}
        {/*    <SpeakerLeg/>*/}
        {/*</div>*/}
      </Speaker>
    </div>
  );
}
