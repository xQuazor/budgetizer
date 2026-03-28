export default function SpeakerLeg({ className = "" }) {
  return (
    <div
      className={`rounded-full ${className} w-16 h-2`}
      style={{
        background: 'radial-gradient(ellipse at 40% 35%, #b0b0b0 0%, #6e6e6e 50%, #4a4a4a 100%)',
        boxShadow: 'inset 0px 2px 4px rgba(255,255,255,0.15), 0px 2px 6px rgba(0,0,0,0.4)',
      }}
    />
  );
}
