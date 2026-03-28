// Speaker grill pattern — b=bright, d=dark, s=special highlight
import {IMG} from "../images.ts";

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

export default function SpeakerGrill() {
    return (
        <div className="flex flex-col gap-2.5 self-center scale-95 pointer-events-none">
            {GRILL.map((row, ri) => (
                <div key={ri} className="flex gap-2.5 items-center">
                    {row.map((cell, ci) => (
                        <img
                            key={ci}
                            alt=""
                            className="block w-2.5 h-2.5 shrink-0"
                            src={cell === 's' ? IMG.holeSpec : cell === 'd' ? IMG.holeDark : IMG.holeBright}
                        />
                    ))}
                </div>
            ))}
        </div>
    );
}