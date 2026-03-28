import { useState, useEffect } from "react";
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
  const [smoothActive, setSmoothActive] = useState(false);

  const [bitDepth1, setBitDepth1] = useState(1);
  const [rate1, setRate1] = useState(1);
  const [bitDepth2, setBitDepth2] = useState(1);
  const [rate2, setRate2] = useState(1);
  const [bitDepth3, setBitDepth3] = useState(1);
  const [rate3, setRate3] = useState(1);

  useEffect(() => { setParameter("bitDepth1", bitDepth1); }, [bitDepth1]);
  useEffect(() => { setParameter("rate1",     rate1);     }, [rate1]);
  useEffect(() => { setParameter("bitDepth2", bitDepth2); }, [bitDepth2]);
  useEffect(() => { setParameter("rate2",     rate2);     }, [rate2]);
  useEffect(() => { setParameter("bitDepth3", bitDepth3); }, [bitDepth3]);
  useEffect(() => { setParameter("rate3",     rate3);     }, [rate3]);

  const settingContainerStyles =
    "relative flex flex-col items-center justify-center w-fit gap-4 rounded-xl px-3 py-4";
  const knobContainerStyles =
    "flex flex-col items-center justify-center gap-2 w-fit";

  return (
    <div className={"w-full h-full"}>
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
                  active={smoothActive}
                  onClick={() => setSmoothActive((v) => !v)}
                />
                <div className={knobContainerStyles}>
                  <Knob
                    label="Depth"
                    min={1}
                    max={16}
                    step={1}
                    value={bitDepth1}
                    setValue={setBitDepth1}
                  />
                  <Knob
                    label="Rate"
                    min={1}
                    max={16}
                    step={1}
                    value={rate1}
                    setValue={setRate1}
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
                  label="Smooth"
                  active={smoothActive}
                  onClick={() => setSmoothActive((v) => !v)}
                />
                <div className={knobContainerStyles}>
                  <Knob
                    label="Depth"
                    min={1}
                    max={16}
                    step={1}
                    value={bitDepth2}
                    setValue={setBitDepth2}
                  />
                  <Knob
                    label="Rate"
                    min={1}
                    max={16}
                    step={1}
                    value={rate2}
                    setValue={setRate2}
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
                  label="Smooth"
                  active={smoothActive}
                  onClick={() => setSmoothActive((v) => !v)}
                />
                <div className={knobContainerStyles}>
                  <Knob
                    label="Depth"
                    min={1}
                    max={16}
                    step={1}
                    value={bitDepth3}
                    setValue={setBitDepth3}
                  />
                  <Knob
                    label="Rate"
                    min={1}
                    max={16}
                    step={1}
                    value={rate3}
                    setValue={setRate3}
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
