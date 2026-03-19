import { useState, useEffect } from 'react';
import { setParameter, onParameterChange } from '../bridge';

export default function Toggle({ paramId, label }) {
    const [checked, setChecked] = useState(false);

    useEffect(() => {
        return onParameterChange(paramId, (v) => setChecked(v >= 0.5));
    }, [paramId]);

    const handleChange = (e) => {
        const v = e.target.checked ? 1.0 : 0.0;
        setChecked(e.target.checked);
        setParameter(paramId, v);
    };

    return (
        <label className="toggle-container">
            <input type="checkbox" checked={checked} onChange={handleChange} />
            <span className="toggle-label">{label}</span>
        </label>
    );
}
