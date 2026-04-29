export const Speaker = ({children, className, style}) => {
    return <div className={`${className} relative py-2 flex flex-col gap w-fit pt-16 pb-4 mx-4 px-6 h-fit scale-100`}
                style={style}>{children}</div>
}