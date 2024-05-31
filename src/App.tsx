import { useEffect, useState } from 'react'
import { CardCharLine, ChartLine } from './components/cards'
import { CCol, CContainer, CFormLabel, CFormRange, CRow, CWidgetStatsA } from '@coreui/react'
import './App.css'
import ToggleSwitch from './components/ToggleSwitch';
import NavBar from './components/NavBar';
import MenuUsers from './components/MenuUsers';

interface StateProps {
  temperature: string;
  range: number;
}

const initialState: StateProps = {
  range: 0,
  temperature: 'NAN',
}

function App() {

  const [state, setState] = useState<StateProps>(initialState);

  const handleGetTemperatura = async () => {
    let response = await fetch("http://192.168.254.46/temperature", {
      method: "GET",
      // headers: headersList
    });

    let data = await response.text();
    console.log("data ", data);

    // if (!data.ok) {
    //   console.log("error ")
    // } else {
    //   // setTemperature(data.json())
    //   console.log(data.json())
    // }
  }

  useEffect(() => {
    handleGetTemperatura()
  }, []);

  return (
    <div className='bg-dark text-white px-0 pb-3'>
      <NavBar />
      <CRow className='m-0 mt-3'>
        <CCol xl={6} xs={12}>
          <CardCharLine temperature='4' />
        </CCol>
        <CCol className='mb-3' xl={6} xs={12}>
          <CWidgetStatsA
            color="danger"
            title="Gerenciais"
            style={{ height: '250px' }}
            value={
              <span className="fs-3">Controles{' '}</span>
            }
            chart={
              <CContainer fluid>
                <ToggleSwitch label='Ligar / Desligar' valueOn='ON' valueOff='OFF' />
                <CFormLabel className='ps-2 mt-2'>Ajustar Temperatura</CFormLabel>
                <CContainer className='d-flex' fluid>
                  <CFormRange id="rangeTemp" min={-100} max={100} value={state.range} onChange={({ target }) => setState({ ...state, range: +target.value })} />
                  <CFormLabel className='ps-2'>{state.range}</CFormLabel>
                </CContainer>
              </CContainer>
            }
          />
        </CCol>
        <CCol className='mb-3' xl={6} xs={12}>
          <ChartLine />
        </CCol>
        <CCol xl={6} xs={12}>
          <MenuUsers />
        </CCol>
      </CRow>
    </div>
  )
}

export default App
