import Circle from "./Circle.jsx";

export default function ModeButton({
  label,
  active = false,
  onClick,
  scale = 0.5,
}) {
  return (
    <button
      onClick={onClick}
      className="block gap-0.5 cursor-pointer select-none bg-transparent border-none p-0 w-fit"
      style={{ zoom: scale }}
    >
      <div className={"flex flex-col items-center justify-center"}>
        <div
          className="relative size-10 overflow-hidden rounded-[16rem]"
          style={{ opacity: active ? 1 : 0.5 }}
        >
          <Circle
            key={"Button Top"}
            wrapperClasses={
              "z-2 top-1/2 left-1/2 translate-x-[calc(-50%+2px)] translate-y-[calc(-50%+2px)]"
            }
            circleClasses={"h-7 w-7 shadow-[inset_-4px_4px_4px_0_rgba(255,255,255,0.25)]"}
            fill="#70A0C3"
            strokeWidth={2}
            strokeColor="#ffffff"
            borderOpacity={0.1}
            borderBlendMode={"normal"}
          />
          <Circle
            key={"Button Side"}
            wrapperClasses={
              "z-1 top-1/2 left-1/2 -translate-x-1/2 -translate-y-1/2"
            }
            circleClasses={"h-8 w-8"}
            fill="#4a90d9"
            strokeWidth={2}
            strokeColor="#000000"
            borderOpacity={0.7}
          />
          {/*<Circle key={"Button Shading"} fill="#4a90d9" strokeWidth={2} strokeColor="#ffffff" borderOpacity={0.4} />*/}
        </div>
        <p
          className="font-['Inter',sans-serif] font-normal text-[16px] leading-normal whitespace-nowrap text-center"
          style={{ color: active ? "#fff" : "#ddd" }}
        >
          {label}
        </p>
      </div>
    </button>
  );
}
