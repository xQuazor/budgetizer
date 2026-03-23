import { useState, useRef, useCallback } from 'react';
import Knob from './components/Knob';

const ANTENNA_GRAD = 'linear-gradient(270deg, rgb(146,146,146) 3.37%, rgb(27,27,27) 7.69%, rgb(122,122,122) 21.64%, rgb(217,217,217) 37.02%, rgb(169,169,169) 50.48%, rgb(27,27,27) 90.39%, rgb(79,79,79) 98.08%)';

const IMG = {
  holeBright:  'https://www.figma.com/api/mcp/asset/16aeb77b-5630-464a-99f4-6cb71ab6a5a6',
  holeDark:    'https://www.figma.com/api/mcp/asset/3248c2a4-38c7-4697-b08b-0e312273a344',
  holeSpec:    'https://www.figma.com/api/mcp/asset/adb50f2e-e07d-432e-a8c7-c8d007a8d337',
  iconSmooth:  'https://www.figma.com/api/mcp/asset/1f3c4b97-b987-4ba1-a483-ce58177e9683',
  iconRadio:   'https://www.figma.com/api/mcp/asset/903dccc5-3c01-483f-9efc-3afebb6ae1df',
  legs:        'https://www.figma.com/api/mcp/asset/3cb71e73-797b-4ed9-bfef-064bf0e5f44d',
  shading2:    'https://www.figma.com/api/mcp/asset/3cd222fd-4bd1-429a-8791-c0e20be7347d',
  shading1:    'https://www.figma.com/api/mcp/asset/313661ab-818c-4df1-8294-cdcca9e38121',
};

// Speaker grill pattern — b=bright, d=dark, s=special highlight
const GRILL = [
  Array(12).fill('b'),
  Array(12).fill('b'),
  Array(12).fill('b'),
  Array(12).fill('b'),
  [...Array(4).fill('b'), ...Array(4).fill('d'), ...Array(4).fill('b')],
  [...Array(3).fill('b'), ...Array(6).fill('d'), ...Array(3).fill('b')],
  [...Array(3).fill('b'), ...Array(6).fill('d'), ...Array(2).fill('b'), 's'],
  [...Array(3).fill('b'), ...Array(6).fill('d'), ...Array(3).fill('b')],
  [...Array(4).fill('b'), ...Array(4).fill('d'), ...Array(4).fill('b')],
  Array(12).fill('b'),
  Array(12).fill('b'),
  Array(12).fill('b'),
  Array(12).fill('b'),
];

// Knob with local React state (for params not yet wired to JUCE)
function LocalKnob({ label, min = 0, max = 1, step = 0.01, defaultValue }) {
  const [value, setValue] = useState(defaultValue ?? min);
  const dragging = useRef(false);
  const startY = useRef(0);
  const startVal = useRef(0);

  const onMove = useCallback((e) => {
    if (!dragging.current) return;
    const delta = (startY.current - e.clientY) * (max - min) / 200;
    const clamped = Math.min(max, Math.max(min, startVal.current + delta));
    setValue(Math.round(clamped / step) * step);
  }, [min, max, step]);

  const onUp = useCallback(() => {
    dragging.current = false;
    document.removeEventListener('mousemove', onMove);
    document.removeEventListener('mouseup', onUp);
  }, [onMove]);

  const onDown = useCallback((e) => {
    dragging.current = true;
    startY.current = e.clientY;
    startVal.current = value;
    document.addEventListener('mousemove', onMove);
    document.addEventListener('mouseup', onUp);
  }, [value, onMove, onUp]);

  const rotation = ((value - min) / (max - min)) * 270 - 135;

  return (
    <div className="knob-container">
      <div className="knob" onMouseDown={onDown} style={{ transform: `rotate(${rotation}deg)` }}>
        <div className="knob-indicator" />
      </div>
      <span className="knob-label">{label}</span>
      <span className="knob-value">{step >= 1 ? Math.round(value) : value.toFixed(2)}</span>
    </div>
  );
}

function ModeButton({ icon, label, active, onClick }) {
  return (
    <button className={`mode-btn${active ? ' mode-btn--active' : ''}`} onClick={onClick}>
      <div className="mode-btn-icon">
        <img alt="" src={icon} />
      </div>
      <span>{label}</span>
    </button>
  );
}

function SpeakerGrill() {
  return (
    <div className="speaker-grill">
      {GRILL.map((row, ri) => (
        <div key={ri} className="grill-row">
          {row.map((cell, ci) => (
            <img
              key={ci}
              alt=""
              className="grill-hole"
              src={cell === 's' ? IMG.holeSpec : cell === 'd' ? IMG.holeDark : IMG.holeBright}
            />
          ))}
        </div>
      ))}
    </div>
  );
}

export default function App() {
  const [smoothActive, setSmoothActive] = useState(false);
  const [radioActive,  setRadioActive]  = useState(true);
  const [syncActive,   setSyncActive]   = useState(false);

  return (
    <div className="plugin-root">

      {/* Accessories */}
      <div className="speaker-legs">
        <img alt="" src={IMG.legs} />
      </div>
      <div className="antenna">
        <div className="ant-pole"   style={{ background: ANTENNA_GRAD }} />
        <div className="ant-mount"  style={{ background: ANTENNA_GRAD }} />
        <div className="ant-neck1"  style={{ background: ANTENNA_GRAD }} />
        <div className="ant-neck2"  style={{ background: ANTENNA_GRAD }} />
        <div className="ant-shaft"  style={{ background: ANTENNA_GRAD }} />
      </div>

      {/* Body layers */}
      <div className="body-3">
        <div className="body-2">
          <div className="body-shading body-shading--2">
            <img alt="" src={IMG.shading2} />
          </div>
          <div className="body-1">
            <div className="body-shading body-shading--1">
              <img alt="" src={IMG.shading1} />
            </div>
          </div>
        </div>
      </div>

      {/* UI overlay */}
      <div className="radio-ui">

        {/* Branding bar */}
        <div className="branding">
          <div className="branding-left">
            <span className="brand-name">BUDGETIZER</span>
            <span className="brand-sub">Audio Degradation Unit</span>
          </div>
          <span className="brand-author">daveys audio</span>
        </div>

        {/* Controls + speaker row */}
        <div className="controls-row">

          {/* Left panel: Smooth / Rate / Bits */}
          <div className="control-panel">
            <ModeButton icon={IMG.iconSmooth} label="Smooth" active={smoothActive} onClick={() => setSmoothActive(v => !v)} />
            <div className="panel-knobs">
              <div className="compact-knob">
                <Knob paramId="reductionFactor" label="Rate" min={1} max={32} step={1} />
              </div>
              <div className="compact-knob">
                <Knob paramId="bitDepth" label="Bits" min={1} max={16} step={1} />
              </div>
            </div>
          </div>

          {/* Middle panel: Radio / Burst & Drift / Depth */}
          <div className="control-panel">
            <ModeButton icon={IMG.iconRadio} label="Radio" active={radioActive} onClick={() => setRadioActive(v => !v)} />
            <div className="panel-knobs">
              <div className="compact-knob">
                <Knob paramId="cutoff" label="Burst & Drift" min={0} max={20000} step={100} />
              </div>
              <div className="compact-knob">
                <LocalKnob label="Depth" min={0} max={1} step={0.01} defaultValue={0.25} />
              </div>
            </div>
          </div>

          <SpeakerGrill />

          {/* Right panel: Sync / Mix */}
          <div className="control-panel">
            <ModeButton icon={IMG.iconSmooth} label="Sync" active={syncActive} onClick={() => setSyncActive(v => !v)} />
            <div className="panel-knobs">
              <div className="compact-knob">
                <LocalKnob label="Mix" min={0} max={1} step={0.01} defaultValue={0.2} />
              </div>
            </div>
          </div>

        </div>
      </div>
    </div>
  );
}
