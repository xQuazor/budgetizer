import aluminiumTexture from "../assets/aluminium_texture.jpg";
import topLayerShading from "../assets/Top Layer Shading.png";
import middleLayerShading from "../assets/Middle Layer Shading.png";
export const Body = ({ children }) => {
  return (
    <div
      key={"Back Layer"}
      className={"relative w-fit p-2 bg-[#A7DCFF] rounded-[60px] h-fit"}
      style={{ boxShadow: "inset 4px -4px 4px 0px rgba(0,0,0,0.25)" }}
    >
      <div className="absolute inset-0 rounded-[60px] border border-[rgba(110,110,110,0.5)] mix-blend-overlay pointer-events-none" />
      <div
        className="absolute inset-0 rounded-[60px] pointer-events-none"
        style={{
          backgroundImage: `url(${aluminiumTexture})`,
          backgroundSize: "cover",
          opacity: 0.2,
          mixBlendMode: "multiply",
        }}
      />
      <div
        key={"Middle Layer"}
        className={"relative w-fit p-2.75 rounded-[60px] bg-[#80B6DB]"}
        style={{ boxShadow: "inset 4px -4px 7px 2px rgba(0,0,0,0.25), -4px 4px 6px 0px rgba(0,0,0,0.25)" }}
      >
        <div className="absolute inset-0 rounded-[60px] border border-[rgba(0,0,0,0.5)] mix-blend-soft-light pointer-events-none" />
        <div
          className="absolute inset-0 rounded-[60px] pointer-events-none"
          style={{
            backgroundImage: `url(${aluminiumTexture})`,
            backgroundSize: "cover",
            opacity: 0.2,
            mixBlendMode: "multiply",
          }}
        />
        <div
          className="absolute inset-0 rounded-[60px] pointer-events-none"
          style={{
            backgroundImage: `url(${middleLayerShading})`,
            backgroundSize: "cover",
            opacity: 1,
            mixBlendMode: "hard-light",
          }}
        />
        <div
          key={"Front Layer"}
          className={`relative w-fit rounded-[50px] bg-[#7AA9CC]`}
        >
          <div className="absolute inset-0 rounded-[48px] border border-[rgba(255,255,255,0.35)] mix-blend-overlay pointer-events-none" />
          <div
            className="absolute inset-0 rounded-[48px] pointer-events-none"
            style={{
              backgroundImage: `url(${aluminiumTexture})`,
              backgroundSize: "cover",
              opacity: 0.25,
              mixBlendMode: "multiply",
            }}
          />
          <div
            className="absolute inset-0 rounded-[48px] pointer-events-none"
            style={{
              backgroundImage: `url(${topLayerShading})`,
              backgroundSize: "cover",
              opacity: 1,
              mixBlendMode: "soft-light",
            }}
          />
          {children}
        </div>
      </div>
    </div>
  );
};
