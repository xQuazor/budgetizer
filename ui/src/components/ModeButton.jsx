import Circle from "./Circle.jsx";

export default function ModeButton({label, active = false, onClick, scale = 1.5 }) {
    return (
        <button
            onClick={onClick}
            className="block gap-0.5 cursor-pointer select-none bg-transparent border-none p-0 w-fit"
            style={{ transform: `scale(${scale})`, transformOrigin: 'top left' }}
        >
            <div className={"flex flex-col items-center justify-center"}>
                <div
                    className="relative shrink-0 size-8 overflow-hidden rounded-[16rem]"
                    style={{ opacity: active ? 1 : 0.5 }}
                >
                    <Circle key={"Button Top"} className={"absolute right-0 bottom-0 h-6 w-6 z-2"} fill="#70A0C3" strokeWidth={2} strokeColor="#ffffff" borderOpacity={0.1} />
                    <Circle key={"Button Side"} className={"absolute right-0 bottom-0 h-8 w-8 z-1"} fill="#70A0C3" strokeWidth={2} strokeColor="#000000" borderOpacity={0.7} />
                    {/*<Circle key={"Button Shading"} fill="#4a90d9" strokeWidth={2} strokeColor="#ffffff" borderOpacity={0.4} />*/}
                </div>
                <p
                    className="font-['Inter',sans-serif] font-normal text-[8px] leading-normal whitespace-nowrap text-center"
                    style={{ color: active ? '#fff' : '#ddd' }}
                >
                    {label}
                </p>
            </div>
        </button>
    );
}
