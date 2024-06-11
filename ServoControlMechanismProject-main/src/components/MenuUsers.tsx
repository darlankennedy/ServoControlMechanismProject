import { CAvatar, CCol, CContainer, CFormLabel, CRow, CWidgetStatsA, CWidgetStatsF } from "@coreui/react"
import Avatar1 from "../assets/avatar1.jpg";
import Avatar2 from "../assets/avatar2.jpg";
import kennedy from "../assets/kennedy.jpeg";
import Avatar3 from "../assets/janderson-weller.jpeg";

let data = [
    { user: "Waleria Rodrigues", avatar: Avatar1, status: "danger" },
    { user: "Darlan Kennedy", avatar: kennedy, status: "success" },
    { user: "Robert Pinto", avatar: Avatar2, status: "success" },
    { user: "Janderson Weller", avatar: Avatar3, status: "danger" },
]

function getRandomTimeBeforeNow() {
    const now = new Date();
    const startOfDay = new Date(now.getFullYear(), now.getMonth(), now.getDate(), 0, 0, 0, 0);

    // Get a random time in milliseconds between startOfDay and now
    const randomTime = new Date(startOfDay.getTime() + Math.random() * (now.getTime() - startOfDay.getTime()));

    const hours = randomTime.getHours().toString().padStart(2, '0');
    const minutes = randomTime.getMinutes().toString().padStart(2, '0');
    const seconds = randomTime.getSeconds().toString().padStart(2, '0');

    return `${hours}:${minutes}:${seconds}`;
}


const handleDate = () => {
    let list = [];

    for (let i = 0; i < data.length; i++) {
        list.push({
            ...data[i],
            date: getRandomTimeBeforeNow()
        });
    }

    return list;
}

const MenuUsers = () => {
    return (
        <CWidgetStatsA
            style={{ minHeight: '250px', paddingBottom: '10px' }}
            chart={
                <CContainer fluid>
                    <CFormLabel className="fw-semibold">Registro de últimos Acessos</CFormLabel>
                    <CRow className="m-0 align-items- position-relative">
                        {
                            handleDate().map((dt, index) => (
                                <CCol md={6} xs={12} className="p-0 pe-1">
                                    <CWidgetStatsF
                                        style={{ height: '90px' }}
                                        className="mb-1"
                                        color="primary"
                                        icon={<CAvatar src={dt.avatar} status={dt.status} />}
                                        title={dt.user}
                                        value={`Acesso: ${dt.date}`} />
                                </CCol>
                            ))
                        }
                    </CRow>
                </CContainer>
            }
        />
    );
}

export default MenuUsers;