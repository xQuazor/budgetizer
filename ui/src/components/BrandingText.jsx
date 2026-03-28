export default function BrandingText() {
  return (
    <div
      className="flex items-center justify-between w-full"
      style={{ filter: 'drop-shadow(0px -1px 3.6px rgba(0,0,0,0.15))' }}
    >
      <div className="flex flex-col">
        <span
          style={{
            fontFamily: "'Fira Code', monospace",
            fontWeight: 600,
            fontSize: '18px',
            color: '#d8d8d8',
            lineHeight: 'normal',
          }}
        >
          BUDGETIZER
        </span>
        <span
          style={{
            fontFamily: "'Fira Code', monospace",
            fontWeight: 300,
            fontSize: '8px',
            color: '#c9c9c9',
            whiteSpace: 'nowrap',
          }}
        >
          Audio Degradation Unit
        </span>
      </div>
      <span
        style={{
          fontFamily: "'Fira Code', monospace",
          fontWeight: 400,
          fontSize: '10px',
          color: '#d8d8d8',
          whiteSpace: 'nowrap',
          textAlign: 'right',
        }}
      >
        daveys audio
      </span>
    </div>
  );
}
