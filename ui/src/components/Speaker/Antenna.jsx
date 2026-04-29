const ANTENNA_GRAD = 'linear-gradient(270deg, rgb(146,146,146) 3.37%, rgb(27,27,27) 7.69%, rgb(122,122,122) 21.64%, rgb(217,217,217) 37.02%, rgb(169,169,169) 50.48%, rgb(27,27,27) 90.39%, rgb(79,79,79) 98.08%)';

export default function Antenna() {
    return (
        <div className={`relative inset-0 pointer-events-none w-fit block  scale-75 -top-18`}>
            <div className="absolute left-[145.2px] top-[29px] w-[3.6px] h-[40px] rounded-[4px]"
                 style={{background: ANTENNA_GRAD}}/>
            <div className="absolute left-[141px]   top-[24px] w-[12px]   h-[10px] rounded-[1px]"
                 style={{background: ANTENNA_GRAD}}/>
            <div className="absolute left-[142.8px] top-[67px] w-[8.4px]  h-[19px] rounded-[4px]"
                 style={{background: ANTENNA_GRAD}}/>
            <div className="absolute left-[141.6px] top-[71px] w-[10.8px] h-[19px] rounded-[4px]"
                 style={{background: ANTENNA_GRAD}}/>
            <div className="absolute left-[141px]   top-[75px] w-[12px]   h-[55px] rounded-[4px]"
                 style={{background: ANTENNA_GRAD}}/>
        </div>
    )
}