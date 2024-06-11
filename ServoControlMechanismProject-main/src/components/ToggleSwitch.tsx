import { useEffect } from "react";

interface SwitchProps {
    label: string;
    valueOn: string;
    valueOff: string;
}

const ToggleSwitch = ({ label, valueOn, valueOff }: SwitchProps) => {
    useEffect(() => {
        // Acessa o elemento :root
        const root = document.documentElement;
        root.style.setProperty('--variable-on', `'${valueOn}'`);
        root.style.setProperty('--variable-off', `'${valueOff}'`);
        
    }, []);
    return (
        <div className="container">
            {label}{" "}
            <div className="toggle-switch">
                <input type="checkbox" className="checkbox"
                    name={label} id={label} />
                <label className="label" htmlFor={label}>
                    <span className="inner" />
                    <span className="switch" />
                </label>
            </div>
        </div>
    );
};

export default ToggleSwitch;