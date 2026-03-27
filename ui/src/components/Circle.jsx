export default function Circle({
                                   borderBlendMode="normal",
                                   fill = 'transparent',
                                   strokeWidth = 1,
                                   strokeColor = '#ffffff',
                                   borderOpacity = 1,
                                   wrapperClasses = '',
                                   circleClasses = ''
                               }) {
    const hex = strokeColor.replace('#', '');
    const r = parseInt(hex.substring(0, 2), 16);
    const g = parseInt(hex.substring(2, 4), 16);
    const b = parseInt(hex.substring(4, 6), 16);

    return (
        <div className={`absolute ${wrapperClasses}`}>
            <div className={`rounded-full ${circleClasses}`} style={{ backgroundColor: fill }}>
                <div
                    className="absolute inset-0 rounded-full box-border"
                    style={{
                        border: `${strokeWidth}px solid rgba(${r}, ${g}, ${b}, ${borderOpacity})`,
                        mixBlendMode: borderBlendMode,
                    }}
                />
            </div>
        </div>

    );
}
