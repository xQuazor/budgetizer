// Speaker grill pattern — b=bright, d=dark, s=special highlight


const GRILL = [
    Array(12).fill('b'),
    Array(12).fill('b'),
    Array(12).fill('b'),
    Array(12).fill('b'),
    [...Array(4).fill('b'), ...Array(4).fill('d'), ...Array(4).fill('b')],
    [...Array(3).fill('b'), ...Array(6).fill('d'), ...Array(3).fill('b')],
    [...Array(3).fill('b'), ...Array(6).fill('d'), ...Array(3).fill('b')],
    [...Array(3).fill('b'), ...Array(6).fill('d'), ...Array(3).fill('b')],
    [...Array(4).fill('b'), ...Array(4).fill('d'), ...Array(4).fill('b')],
    Array(12).fill('b'),
    Array(12).fill('b'),
    Array(12).fill('b'),
    Array(12).fill('b'),
];

export default function SpeakerGrill() {
    return (
        <div className="flex flex-col gap-2.5 self-center scale-95 pointer-events-none">
            {GRILL.map((row, ri) => (
                <div key={ri} className="flex gap-2.5 items-center">
                    {row.map((cell, ci) => (
                        <div
                            key={ci}
                            className="block w-2 h-2 shrink-0 rounded-full"
                            style={cell === 'd' || cell === 's' ? {
                                background: "#677077",
                                boxShadow: "-1px -1px 0.5px 0 #9EA8B0 inset, 2px 5px 7.1px 1px #1E1E1E inset",
                            } : {
                                background: "rgba(255, 255, 255, 0.01)",
                                boxShadow: "-1px -1px 0.5px 0 rgba(255, 255, 255, 0.15) inset, 2px 1px 1.9px 0 rgba(0, 0, 0, 0.25) inset",
                            }}
                        />
                    ))}
                </div>
            ))}
        </div>
    );
}