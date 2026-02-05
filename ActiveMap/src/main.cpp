//
//  main.cpp
//  ActiveMap
//
//  Created by DiCode77.
//

#include "main.hpp"

bool MyApp::OnInit() {
    AcMap *prog = new AcMap(wxT("AcMap"), wxDefaultPosition, wxSize(600, 500));
    prog->Show(true);
    return true;
}

wxIMPLEMENT_APP(MyApp);
