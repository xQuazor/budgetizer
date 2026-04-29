import noiseTexture from "@/assets/noise.png";
import studioBackground from "@/assets/Studio Background.jpg";

export default function Background({className}) {
    return (
        <>
            <img
                src={noiseTexture}
                alt={"noise texture"}
                className={`absolute mix-blend-overlay w-full h-full z-50 pointer-events-none brightness-75 opacity-100 ${className}`}
            />
            <img
                src={studioBackground}
                alt={"studio background"}
                className={`absolute brightness-100 left-0 right-0 bottom-0 top-8.25 w-full h-[calc(100%-2.0625rem)] object-fill pointer-events-none ${className}`}
            />
        </>
    )

}