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
import noise from "./assets/noise.png"
import studioBackground from "./assets/Studio Background.jpg"
export default function App() {
  const containerRef = useRef(null);

  const [licenseValid, setLicenseValid] = useState(false);

  useEffect(() => {
    return onLicenseStatus((status) => setLicenseValid(status.valid));
  }, []);

  const [bypass, setBypass] = useState(false);
  const [smooth, setSmooth] = useState(false);
  const [radio, setRadio] = useState(true);
  const [sync, setSync] = useState(false);

  const [externalAudio, setExternalAudio] = useState(false);

  const [bitDepth, setBitDepth] = useState(8);
  const [rate, setRate] = useState(5);
  const [depth, setDepth] = useState(0.0);
  const [interval, setInterval] = useState(2);
  const [drive, setDrive] = useState(2.0);
  const [mix, setMix] = useState(1);

  const loadPreset = (p) => {
    setSmooth(p.smooth);
    setRadio(p.radio);
    setBitDepth(p.bitDepth);
    setRate(p.rate);
    setDepth(p.depth);
    setDrive(p.drive);
    setMix(p.mix);
    setInterval(p.interval ?? 2);
  };

  // Bypass
  useEffect(() => { setParameter("bypass", bypass); }, [bypass]);

  // Master
  useEffect(() => { setParameter("masterMix",     mix);     }, [mix]);
  useEffect(() => { setParameter("drive",     drive);     }, [drive]);

  // Digitizer
  useEffect(() => { setParameter("sampleReductionRate",     rate);     }, [rate]);
  useEffect(() => { setParameter("bitDepth",     bitDepth);     }, [bitDepth]);
  useEffect(() => { setParameter("smooth",     smooth);     }, [smooth]);

  // Radio
  useEffect(() => { setParameter("radio",          radio);        }, [radio]);
  useEffect(() => { setParameter("depth",          depth);        }, [depth]);
  useEffect(() => { setParameter("interval",         interval);      }, [interval]);
  useEffect(() => { setParameter("sync",            sync);          }, [sync]);

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
    <div key={"ApplicationContainer"} ref={containerRef} className={"w-fit h-fit relative flex flex-col"}>
      <img src={noise} alt={"noise texture"} className={`absolute mix-blend-overlay w-full h-full z-50 pointer-events-none opacity-100 ${licenseValid ? "" : "blur-xs"}`}/>
      <img src={studioBackground} alt={"studio background"} className={`absolute left-0 right-0 bottom-0 top-8.25 w-full h-[calc(100%-2.0625rem)] object-fill pointer-events-none ${licenseValid ? "" : "blur-xs"}`} />
      <Toolbar presets={PRESETS} onPresetLoad={loadPreset} bypass={bypass} onBypassToggle={() => setBypass((v) => !v)} />
      <Speaker className={`relative py-2 flex flex-col gap w-fit pt-16 pb-4 mx-4 h-fit scale-100 ${licenseValid ? "" : "blur-xs"}`}
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
              <div className={`${settingContainerStyles}`} key={"SettingContainer"}>
                <div
                    className={`${smooth? "bg-black" : "bg-transparent"}`}
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
                    label="Chaos"
                    unit={"%"}
                    min={0.0}
                    max={1}
                    step={0.01}
                    presentationValueMultiplier={100}
                    value={depth}
                    setValue={setDepth}
                  />
                  <Knob
                    label="Interval"
                    min={0}
                    max={4}
                    step={1}
                    labels={["1/16","1/8","1/4","1/2","1/1"]}
                    value={interval}
                    setValue={setInterval}
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
                {/*<ModeButton*/}
                {/*    label="External Audio"*/}
                {/*    value={externalAudio}*/}
                {/*    onClick={() => setExternalAudio((v) => !v)}*/}
                {/*/>*/}
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
                    presentationValueMultiplier={100}
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
