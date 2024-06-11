import { cilArrowBottom } from "@coreui/icons";
import CIcon from "@coreui/icons-react";
import { CWidgetStatsA } from "@coreui/react";
import {
    Chart as ChartJS,
    CategoryScale,
    LinearScale,
    PointElement,
    LineElement,
    Title,
    Tooltip,
    Legend,
} from 'chart.js';
import { useState } from "react";
import { Line } from 'react-chartjs-2';

export function formatHour(date: Date) {
    let hoursDate = date.getHours();
    let hours = `${hoursDate}`;
    // Adiciona zero à esquerda se necessário
    if (hoursDate < 10) hours = '0' + hoursDate;
    return `${hours}:00`;
}

const handleInterval = () => {
    let now = new Date();
    let hoursArray = [];
    hoursArray.push(`Temperatura às ${formatHour(new Date(now))}`);

    // Adiciona as 5 horas anteriores ao array
    for (let i = 1; i <= 5; i++) {
        let previousHour = new Date(now);
        previousHour.setHours(now.getHours() - i);
        hoursArray.push(`Temperatura às ${formatHour(previousHour)}`);
    }
    return hoursArray
}

export const CardCharLine = ({ temperature }: { temperature: string }) => {

    ChartJS.register(
        CategoryScale,
        LinearScale,
        PointElement,
        LineElement,
        Title,
        Tooltip,
        Legend
    );

    const options = {
        plugins: {
            legend: {
                display: false,
            },
        },
        maintainAspectRatio: false,
        scales: {
            x: {
                grid: {
                    display: false,
                },
                ticks: {
                    display: false,
                },
            },
            y: {
                min: -10,
                max: 10,
                display: false,
                grid: {
                    display: false,
                },
                ticks: {
                    display: false,
                },
            },
        },
        elements: {
            line: {
                borderWidth: 1,
                tension: 0.4,
            },
            point: {
                radius: 4,
                hitRadius: 10,
                hoverRadius: 4,
            },
        },
    };

    const data = {
        labels: handleInterval(),
        datasets: [
            {
                label: 'Registrado',
                backgroundColor: 'rgba(255,255,255,.2)',
                borderColor: 'rgba(255,255,255,.55)',
                pointBackgroundColor: '#321fdb',
                data: [2, 5, 8, 8.6, 5.1, 5.5, 4],
            },
        ],
    };

    return (
        <CWidgetStatsA
            className="mb-3 w-100"
            style={{ height: '250px' }}
            color="primary"
            value={
                <>
                    <span className="fs-3">{temperature}°{' '}</span>
                    <span className="fs-6 fw-normal">
                        (Celsius <CIcon icon={cilArrowBottom} />)
                    </span>
                </>
            }
            title="Temperatura"
            chart={
                <Line
                    data={data}
                    options={options}
                />
            }
        />
    );
}

export const ChartLine = () => {

    ChartJS.register(
        CategoryScale,
        LinearScale,
        PointElement,
        LineElement,
        Title,
        Tooltip,
        Legend
    );

    const options = {
        responsive: true,
        maintainAspectRatio: false,
        plugins: {
            legend: {
                position: 'top' as const,
            },
            title: {
                display: true,
                text: 'Análise Semanal de Temperaturas',
            },
        },
        elements: {
            line: {
                borderWidth: 1,
                tension: 0.4,
            },
            point: {
                radius: 4,
                hitRadius: 10,
                hoverRadius: 4,
            },
        },
    };

    const labels = ['Domingo', 'Segunda', 'Terça', 'Quarta', 'Quinta', 'Sexta', 'Sábado'];
    const [values,] = useState<number[]>(labels.map(() => Math.floor(Math.random() * 5)));

    const data = {
        labels,
        datasets: [
            {
                label: 'Temperatura Média / dia',
                data: values,
                borderColor: 'rgba(3, 118, 248, 1)',
                backgroundColor: 'rgba(3, 118, 248, 1)',
            },
        ],
    };

    return (
        <CWidgetStatsA
            className="pb-4  w-100"
            style={{ height: '250px' }}
            chart={
                <Line options={options} data={data} />
            }
        />
    );
}