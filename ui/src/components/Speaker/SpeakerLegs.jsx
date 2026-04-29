export default function SpeakerLegs({className = ""}) {
    return (
        <div
            className={
                `${className} absolute flex flex-row w-full bottom-2.75 px-16 pr-24 justify-between`
            }
        >
            <SpeakerLeg/>
            <SpeakerLeg/>
        </div>
    )
}


function SpeakerLeg({className = ""}) {
    return (
        <div
            className={`rounded-full ${className} w-16 h-2`}
            style={{
                background: 'radial-gradient(ellipse at 40% 35%, #707070 0%, #3a3a3a 50%, #1e1e1e 100%)',
                // boxShadow: 'inset 0px 2px 4px rgba(255,255,255,0.15), 0px 2px 6px rgba(0,0,0,0.4)',
            }}
        />
    );
}
