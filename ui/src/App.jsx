import {useState, useEffect, useRef} from "react";
import {setParameter, onLicenseStatus} from "./bridge";
import LicenseOverlay from "./components/LicenseOverlay.jsx";
import Toolbar from "./components/Toolbar.jsx";
import {PRESETS} from "./presets.js";
import Knob from "./components/Speaker/Knob.jsx";
import ModeButton from "./components/ModeButton.jsx";
import SpeakerGrill from "./components/Speaker/SpeakerGrill.jsx";
import BrandingText from "./components/Speaker/BrandingText.jsx";
import Antenna from "./components/Speaker/Antenna.jsx";
import {Body} from "./components/Speaker/Body.jsx";
import {Speaker} from "./components/Speaker/Speaker.jsx";
import SpeakerLegs from "./components/Speaker/SpeakerLegs.jsx";
import ChannelStrip from "@/components/Speaker/ChannelStrip.jsx";
import Background from "@/components/Background.jsx";

export default function App() {
    const containerRef = useRef(null);

    const [licenseValid, setLicenseValid] = useState(false);

    useEffect(() => {
        return onLicenseStatus((status) => setLicenseValid(status.valid));
    }, []);

    const [bypass, setBypass] = useState(false);
    const [noise, setNoise] = useState(true);
    const [smooth, setSmooth] = useState(false);
    const [radio, setRadio] = useState(true);
    const [sync, setSync] = useState(false);

    const [externalAudio, setExternalAudio] = useState(false);

    const [bitDepth, setBitDepth] = useState(8);
    const [rate, setRate] = useState(5);
    const [chaos, setChaos] = useState(0.0);
    const [interval, setInterval] = useState(3);
    const [intervalMs, setIntervalMs] = useState(500);
    const [drive, setDrive] = useState(2.0);
    const [mix, setMix] = useState(1);

    const loadPreset = (p) => {
        setSmooth(p.smooth);
        setRadio(p.radio);
        setBitDepth(p.bitDepth);
        setRate(p.rate);
        setChaos(p.chaos);
        setDrive(p.drive);
        setMix(p.mix);
        setInterval(p.interval ?? 2);
        setIntervalMs(p.intervalMs ?? 500);
    };

    // Bypass
    useEffect(() => {
        setParameter("bypass", bypass);
    }, [bypass]);
    useEffect(() => {
        setParameter("noise", noise);
    }, [noise]);

    // Master
    useEffect(() => {
        setParameter("masterMix", mix);
    }, [mix]);
    useEffect(() => {
        setParameter("drive", drive);
    }, [drive]);

    // Digitizer
    useEffect(() => {
        setParameter("sampleReductionRate", rate);
    }, [rate]);
    useEffect(() => {
        setParameter("bitDepth", bitDepth);
    }, [bitDepth]);
    useEffect(() => {
        setParameter("smooth", smooth);
    }, [smooth]);

    // Radio
    useEffect(() => {
        setParameter("radio", radio);
    }, [radio]);
    useEffect(() => {
        setParameter("chaos", chaos);
    }, [chaos]);
    useEffect(() => {
        setParameter("interval", sync ? interval : intervalMs);
    }, [sync, interval, intervalMs]);
    useEffect(() => {
        setParameter("sync", sync);
    }, [sync]);
    useEffect(() => {
        const off = (sync && interval === 0) || (!sync && intervalMs === 0);
        setParameter("gated", !off);
    }, [sync, interval, intervalMs]);

    // External Input
    useEffect(() => {
        setParameter("useAudioInput", externalAudio);
    }, [externalAudio]);

    useEffect(() => {
        if (!containerRef.current || typeof window.__JUCE__ === "undefined") return;
        requestAnimationFrame(() => {
            const rect = containerRef.current.getBoundingClientRect();
            console.log("rect", rect);
            window.__JUCE__.backend.emitEvent("resize", {
                width: Math.round(rect.width),
                height: Math.round(rect.height),
            });
        });
    }, []);

    return (
        <div
            key={"ApplicationContainer"}
            ref={containerRef}
            className={"w-fit h-fit relative flex flex-col"}
        >
            <Background className={`${licenseValid ? "" : "blur-xs"}`}/>
            <Toolbar
                presets={PRESETS}
                onPresetLoad={loadPreset}
                bypass={bypass}
                onBypassToggle={() => setBypass((v) => !v)}
                sync={sync}
                onSyncToggle={() => setSync((v) => !v)}
                radio={radio}
                onRadioToggle={() => setRadio((v) => !v)}
            />
            <Speaker
                className={`${licenseValid ? "" : "blur-xs"}`}
            >
                <Antenna/>
                <SpeakerLegs/>
                <Body>
                    <div className={"flex flex-col gap-4 p-6 w-fit"}>
                        <BrandingText/>
                        <div className={"flex flex-row gap-4"}>
                            <ChannelStrip
                                button={
                                    <ModeButton
                                        label="Smooth"
                                        value={smooth}
                                        onClick={() => setSmooth((v) => !v)}
                                    />
                                }
                                knob1={<Knob
                                    label="Rate"
                                    unit={"kHz"}
                                    min={1}
                                    max={22}
                                    step={0.05}
                                    value={rate}
                                    setValue={setRate}
                                />}
                                knob2={<Knob
                                    label="Bits"
                                    min={1}
                                    max={24}
                                    step={1}
                                    value={bitDepth}
                                    setValue={setBitDepth}
                                />}
                            />
                            <ChannelStrip
                                button={<ModeButton
                                    label="FM"
                                    value={radio}
                                    onClick={() => setRadio((v) => !v)}
                                />
                                }

                                knob1={<Knob
                                    label="Chaos"
                                    unit={"%"}
                                    min={0.0}
                                    max={1}
                                    step={0.01}
                                    presentationValueMultiplier={100}
                                    value={chaos}
                                    setValue={setChaos}
                                />}
                                knob2={
                                    sync ? (
                                        <Knob
                                            label="Interval"
                                            min={1}
                                            max={5}
                                            step={1}
                                            labels={["1/16", "1/8", "1/4", "1/2", "1/1"]}
                                            zeroLabel="Off"
                                            value={interval}
                                            setValue={setInterval}
                                        />
                                    ) : (
                                        <Knob
                                            label="Interval"
                                            unit="ms"
                                            min={50}
                                            max={2000}
                                            step={1}
                                            zeroLabel="Off"
                                            value={intervalMs}
                                            setValue={setIntervalMs}
                                        />
                                    )
                                }/>
                            <SpeakerGrill/>
                            <ChannelStrip
                                button={
                                    <ModeButton
                                        label="Widen"
                                        value={noise}
                                        onClick={() => setNoise((v) => !v)}
                                    />
                                }
                                knob1={
                                    <Knob
                                        label="Drive"
                                        unit={"x"}
                                        min={1.0}
                                        max={10.0}
                                        step={1.0}
                                        value={drive}
                                        setValue={setDrive}
                                    />
                                }
                                knob2={
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
                                }
                            />
                        </div>
                    </div>
                </Body>
            </Speaker>
            {!licenseValid && (
                <LicenseOverlay onActivated={() => setLicenseValid(true)}/>
            )}
        </div>
    );
}
