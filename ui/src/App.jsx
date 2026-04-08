import { useState, useEffect, useRef } from "react";
import { setParameter, onLicenseStatus } from "./bridge";
import LicenseOverlay from "./components/LicenseOverlay.jsx";
import Toolbar from "./components/Toolbar.jsx";
import { PRESETS } from "./presets.js";
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

  const [licenseValid, setLicenseValid] = useState(false);

  useEffect(() => {
    return onLicenseStatus((status) => setLicenseValid(status.valid));
  }, []);

  const [smooth, setSmooth] = useState(false);
  const [radio, setRadio] = useState(true);
  const [sync, setSync] = useState(false);

  const [externalAudio, setExternalAudio] = useState(false);

  const [bitDepth, setBitDepth] = useState(8);
  const [rate, setRate] = useState(5);
  const [burstDrift, setBurstDrift] = useState(20);
  const [depth, setDepth] = useState(25);
  const [emphasis, setEmphasis] = useState(0.5);
  const [multipathMix, setMultipathMix] = useState(0.3);
  const [multipathDelay, setMultipathDelay] = useState(1.0);
  const [ceiling, setCeiling] = useState(0.7);
  const [drive, setDrive] = useState(2.0);
  const [mix, setMix] = useState(50);

  const loadPreset = (p) => {
    setSmooth(p.smooth);
    setRadio(p.radio);
    setBitDepth(p.bitDepth);
    setRate(p.rate);
    setBurstDrift(p.burstDrift);
    setDepth(p.depth);
    setDrive(p.drive);
    setMix(p.mix);
    setEmphasis(p.emphasis);
    setMultipathMix(p.multipathMix);
    setMultipathDelay(p.multipathDelay);
    setCeiling(p.ceiling);
  };

  // Master
  useEffect(() => { setParameter("masterMix",     mix);     }, [mix]);
  useEffect(() => { setParameter("drive",     drive);     }, [drive]);

  // Digitizer
  useEffect(() => { setParameter("sampleReductionRate",     rate);     }, [rate]);
  useEffect(() => { setParameter("bitDepth",     bitDepth);     }, [bitDepth]);
  useEffect(() => { setParameter("smooth",     smooth);     }, [smooth]);

  // Radio
  useEffect(() => { setParameter("radio",          radio);        }, [radio]);
  useEffect(() => { setParameter("drift",          burstDrift);   }, [burstDrift]);
  useEffect(() => { setParameter("depth",          depth);        }, [depth]);
  useEffect(() => { setParameter("emphasis",       emphasis);     }, [emphasis]);
  useEffect(() => { setParameter("multipathMix",   multipathMix); }, [multipathMix]);
  useEffect(() => { setParameter("multipathDelay", multipathDelay); }, [multipathDelay]);
  useEffect(() => { setParameter("ceiling",        ceiling);      }, [ceiling]);

  // External Input
  useEffect(() => { setParameter("useAudioInput",     externalAudio);     }, [externalAudio]);

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

  const containerBorderStyle = {
        position: "absolute",
        inset: 0,
        borderRadius: "inherit",
        border: "2px solid transparent",
        background:
            "radial-gradient(circle, #000000 0%, #666666 100%) border-box",
        WebkitMask:
            "linear-gradient(#fff 0 0) padding-box, linear-gradient(#fff 0 0)",
        WebkitMaskComposite: "destination-out",
        maskComposite: "exclude",
        mixBlendMode: "screen",
        pointerEvents: "none",
    }
  const settingContainerStyles =
    "relative flex flex-col items-center justify-between w-fit gap-4 rounded-xl px-3 py-4";
  const knobContainerStyles =
    "flex flex-col items-center justify-center gap-2 w-fit";

  return (
    <div key={"ApplicationContainer"} ref={containerRef} className={"w-fit h-fit"} style={{ position: "relative", display: "flex", flexDirection: "column" }}>
      <Toolbar presets={PRESETS} onPresetLoad={loadPreset} />
      <Speaker className={"relative py-2 flex flex-col gap w-fit pt-14 pb-4 mx-4 h-fit"}
      >
        <Antenna className={"-top-18"}/>
        <div className={"absolute flex flex-row w-full bottom-2.75 px-16 justify-between"}>
            <SpeakerLeg/>
            <SpeakerLeg/>
        </div>
        <Body>
          <div className={"flex flex-col gap-4 p-6 w-fit"}>
            <BrandingText />
            <div className={"flex flex-row gap-4"}>
              <div className={settingContainerStyles} key={"SettingContainer"}>
                <div
                  style={containerBorderStyle}
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
                    step={0.05}
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
                    style={containerBorderStyle}
                />
                <ModeButton
                  label="Radio"
                  value={radio}
                  onClick={() => setRadio((v) => !v)}
                />
                <div className={knobContainerStyles}>
                  <Knob
                    label="Gate"
                    unit={"%"}
                    min={1}
                    max={5000}
                    step={1}
                    value={burstDrift}
                    setValue={setBurstDrift}
                  />
                  <Knob
                    label="Chaos"
                    unit={"%"}
                    min={0.01}
                    max={1}
                    step={0.01}
                    value={depth}
                    setValue={setDepth}
                  />
                  <Knob
                    label="Emphasis"
                    min={0}
                    max={1}
                    step={0.01}
                    value={emphasis}
                    setValue={setEmphasis}
                  />
                  <Knob
                    label="Multipath"
                    min={0}
                    max={1}
                    step={0.01}
                    value={multipathMix}
                    setValue={setMultipathMix}
                  />
                  <Knob
                    label="Delay"
                    unit={"ms"}
                    min={0.1}
                    max={3.0}
                    step={0.01}
                    value={multipathDelay}
                    setValue={setMultipathDelay}
                  />
                  <Knob
                    label="Ceiling"
                    min={0.1}
                    max={1.0}
                    step={0.01}
                    value={ceiling}
                    setValue={setCeiling}
                  />
                </div>
              </div>

              <SpeakerGrill />

              <div className={settingContainerStyles} key={"SettingContainer3"}>
                <div
                    style={containerBorderStyle}
                />
                <ModeButton
                  label="Sync"
                  value={sync}
                  onClick={() => setSync((v) => !v)}
                />
                <ModeButton
                    label="External Audio"
                    value={externalAudio}
                    onClick={() => setExternalAudio((v) => !v)}
                />
                <div className={knobContainerStyles}>
                    <Knob
                        label="Drive"
                        unit={"x"}
                        min={1.0}
                        max={10.0}
                        step={1.0}
                        value={drive}
                        setValue={setDrive}
                    />
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
      </Speaker>
      {!licenseValid && <LicenseOverlay onActivated={() => setLicenseValid(true)} />}
    </div>
  );
}
