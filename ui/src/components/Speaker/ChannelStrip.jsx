export default function ChannelStrip({button, knob1, knob2, triangleIcon = false}) {
    const containerBorderStyle = {
        position: "absolute",
        inset: 0,
        borderRadius: "inherit",
        border: "2px solid transparent",
        background: "radial-gradient(circle, #000000 0%, #666666 100%) border-box",
        WebkitMask:
            "linear-gradient(#fff 0 0) padding-box, linear-gradient(#fff 0 0)",
        WebkitMaskComposite: "destination-out",
        maskComposite: "exclude",
        mixBlendMode: "screen",
        pointerEvents: "none",
    };
    const settingContainerStyles =
        "relative flex flex-col items-center justify-between w-fit gap-4 rounded-xl px-3 py-4";
    const knobContainerStyles =
        "flex flex-col items-center justify-center gap-2 w-fit";

    return (
        <div
            className={`${settingContainerStyles}`}
            key={"SettingContainer"}
        >
            <div
                style={containerBorderStyle}
            />
            {button}
            <div className={knobContainerStyles}>
                {knob1}
                {/*<svg className={triangleIcon ? "" : "invisible"} width="8" height="4" viewBox="0 0 12 8" fill="white"*/}
                {/*     xmlns="http://www.w3.org/2000/svg">*/}
                {/*    <path d="M6 8L0 0H12L6 8Z"/>*/}
                {/*</svg>*/}
                {knob2}
            </div>
        </div>
    )
}