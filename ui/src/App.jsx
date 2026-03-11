import Knob   from './components/Knob';
import Toggle from './components/Toggle';

export default function App() {
    return (
        <div className="plugin-ui">
            <h1 className="plugin-title">Degradizer</h1>

            <div className="knobs-row">
                <Knob paramId="bitDepth"       label="Bit Depth"    min={1}   max={16}    step={1}   />
                <Knob paramId="reductionFactor" label="Sample Rate↓" min={1}   max={32}    step={1}   />
                <Knob paramId="cutoff"          label="Cutoff"       min={0}   max={20000} step={10}  />
            </div>

            <div className="toggles-row">
                <Toggle paramId="dithering"   label="Dithering"      />
                <Toggle paramId="linearInterp" label="Linear Interp" />
                <Toggle paramId="polyBlep"    label="PolyBLEP"       />
            </div>
        </div>
    );
}
