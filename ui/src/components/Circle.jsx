export default function Circle({ fill = 'transparent', strokeWidth = 1, strokeColor = '#ffffff', borderOpacity = 1, className = '' }) {
    const hex = strokeColor.replace('#', '');
    const r = parseInt(hex.substring(0, 2), 16);
    const g = parseInt(hex.substring(2, 4), 16);
    const b = parseInt(hex.substring(4, 6), 16);

    return (
        <div
            className={`rounded-full box-border ${className}`}
            style={{
                backgroundColor: fill,
                border: `${strokeWidth}px solid rgba(${r}, ${g}, ${b}, ${borderOpacity})`,
            }}
        />
    );
}
