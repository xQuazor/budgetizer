import Circle from "./Circle.jsx";

export default function ModeButton({ label, value = false, onClick }) {
  if (value) return <PressedState label={label} onClick={onClick} />;
  if (!value) return <DefaultState label={label} onClick={onClick} />;
}

const PressedState = ({ label, onClick }) => {
  return (
    <button
      onClick={onClick}
      className="block gap-0.25 cursor-pointer select-none bg-transparent border-none p-0 w-fit"
    >
      <div className={"flex flex-col items-center justify-center"}>
        <div
          className="relative size-5 overflow-hidden rounded-[16rem]"
          style={{
            opacity: 1,
            filter:
              "drop-shadow(0 0 6px rgba(255,255,255,0.13)) drop-shadow(0 0 2px rgba(255,255,255,0.24))",
          }}
        >
          <Circle
            key={"Button Top"}
            wrapperClasses={
              "z-2 top-1/2 left-1/2 translate-x-[calc(-50%+1px)] translate-y-[calc(-50%+1px)]"
            }
            circleClasses={
              "h-3.5 w-3.5 shadow-[inset_-4px_4px_4px_0_rgba(255,255,255,0.25)]"
            }
            fill="#81B5D9"
            strokeWidth={2}
            strokeColor="#ffffff"
            borderOpacity={0.05}
            borderBlendMode={"normal"}
          />
          <Circle
            key={"Button Side"}
            wrapperClasses={
              "z-1 top-1/2 left-1/2 -translate-x-1/2 -translate-y-1/2"
            }
            circleClasses={"h-3.75 w-3.75"}
            fill="#70A0C3"
            strokeWidth={1}
            strokeColor="#000000"
            borderOpacity={0.05}
          />
        </div>
        {label ? (
          <p
            className="font-['Inter',sans-serif] font-normal text-[8px] leading-normal whitespace-nowrap text-center"
            style={{ color: "#ddd" }}
          >
            {label}
          </p>
        ) : null}
      </div>
    </button>
  );
};

const DefaultState = ({ label, onClick }) => {
  return (
    <button
      onClick={onClick}
      className="block gap-0.25 cursor-pointer select-none bg-transparent border-none p-0 w-fit"
    >
      <div className={"flex flex-col items-center justify-center"}>
        <div
          className="relative size-5 overflow-hidden rounded-[16rem]"
          style={{
            opacity: 1,
          }}
        >
          <Circle
            key={"Button Top"}
            wrapperClasses={
              "z-2 top-1/2 left-1/2 translate-x-[calc(-50%+1px)] translate-y-[calc(-50%+1px)]"
            }
            circleClasses={
              "h-3.5 w-3.5 shadow-[inset_-4px_4px_4px_0_rgba(255,255,255,0.25)]"
            }
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
            circleClasses={"h-4 w-4"}
            fill="#4a90d9"
            strokeWidth={2}
            strokeColor="#000000"
            borderOpacity={0.7}
          />
        </div>
        {label ? (
          <p
            className="font-['Inter',sans-serif] font-normal text-[8px] leading-normal whitespace-nowrap text-center"
            style={{ color: "#ddd" }}
          >
            {label}
          </p>
        ) : null}
      </div>
    </button>
  );
};
