// import body3 from '../assets/body/Radio Body 3.png';
import body2 from '../assets/body/Radio Body 2.png';
// import body1 from '../assets/body/Radio Body 1.png';

export const Body = () => {
    return (
        <div className="relative w-dvw h-dvh">
            {/*<div className={"absolute inset-0 w-full h-full object-contain z-1"}>*/}
            {/*    /!*z-1*!/*/}
            {/*    <img src={body3} alt="" className="absolute inset-0 w-full h-full object-contain z-1 "/>*/}
            {/*</div>*/}
            {/*{*/}
            {/*    missing shadow on this layer*/}
            {/*}*/}
            <div className={"absolute inset-0 w-full h-full object-contain z-2 scale-97"}>
                <img src={body2} alt="" className="absolute inset-0 w-full h-full object-contain z-2 top-0 left-0"/>
            </div>
            {/*<div className={"absolute inset-0 w-full h-full object-contain z-3 scale-93"}>*/}
            {/*    /!*z-3*!/*/}
            {/*    <img src={body1} alt="" className="absolute inset-0 w-full h-full object-contain z-3 top-0 left-0"/>*/}
            {/*</div>*/}
        </div>
    )
}

// className={"p-4"}