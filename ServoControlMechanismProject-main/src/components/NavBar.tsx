import { CCloseButton, CContainer, CNavItem, CNavLink, CNavbar, CNavbarBrand, CNavbarNav, CNavbarToggler, COffcanvas, COffcanvasBody, COffcanvasHeader, COffcanvasTitle } from "@coreui/react"
import { useState } from "react";

const NavBar = () => {
    const [visible, setVisible] = useState<boolean>(false);
    
    return (
        <CNavbar colorScheme="light" className="bg-light">
            <CContainer fluid>
                <CNavbarBrand className="fs-3">Smart Control</CNavbarBrand>
                <CNavbarToggler
                    aria-controls="offcanvasNavbar"
                    aria-label="Toggle navigation"
                    onClick={() => setVisible(!visible)}
                />
                <COffcanvas id="offcanvasNavbar" placement="end" portal={false} visible={visible} onHide={() => setVisible(false)}>
                    <COffcanvasHeader>
                        <COffcanvasTitle>Smart Control</COffcanvasTitle>
                        <CCloseButton className="text-reset" onClick={() => setVisible(false)} />
                    </COffcanvasHeader>
                    <COffcanvasBody>
                        <CNavbarNav>
                            <CNavItem>
                                <CNavLink href="#" active>
                                    Home
                                </CNavLink>
                            </CNavItem>
                            <CNavItem>
                                <CNavLink href="#">Sair</CNavLink>
                            </CNavItem>
                        </CNavbarNav>
                    </COffcanvasBody>
                </COffcanvas>
            </CContainer>
        </CNavbar>
    );
}

export default NavBar;