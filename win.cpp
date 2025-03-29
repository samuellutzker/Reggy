#include "win.h"

wxIMPLEMENT_APP(MyApp);

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_BUTTON(ID_BTN_LOAD, MyFrame::OnBtnLoad)
    EVT_BUTTON(ID_BTN_SAVE, MyFrame::OnBtnSave)
    EVT_BUTTON(ID_BTN_CLEAR, MyFrame::OnBtnClear)
    EVT_BUTTON(ID_BTN_QUIT, MyFrame::OnBtnQuit)
    EVT_TEXT(ID_INP_PTRN, MyFrame::OnChangePtrn)
    EVT_TEXT(ID_INP_DATA, MyFrame::OnChangeData)
    EVT_CHECKBOX(ID_CB_MULTI, MyFrame::OnCheckbox)
    EVT_CHECKBOX(ID_CB_FL_ERE, MyFrame::OnCheckbox)
    EVT_CHECKBOX(ID_CB_FL_ICASE, MyFrame::OnCheckbox)
    EVT_CHECKBOX(ID_CB_FL_NL, MyFrame::OnCheckbox)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyComboBox, wxOwnerDrawnComboBox)
    EVT_COMBOBOX(ID_CO_GROUPS, MyComboBox::OnComboBox)
    EVT_COMBOBOX_DROPDOWN(ID_CO_GROUPS, MyComboBox::OnDropdown)
    EVT_COMBOBOX_CLOSEUP(ID_CO_GROUPS, MyComboBox::OnDropdown)
END_EVENT_TABLE()

// ----- MyComboBox ----

MyComboBox::MyComboBox(wxWindow *parent, wxWindowID id, MyFrame *_mainFrame) : nGroups(0), mainFrame(_mainFrame),
  wxOwnerDrawnComboBox(parent, id, "Total Groups: 0", wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY)
{
    Append("Total Groups: 0");
    SetCursor(wxCURSOR_HAND);
}

MyComboBox::~MyComboBox() {}

void MyComboBox::OnDrawBackground(wxDC& dc, const wxRect& rect, int item, int flags) const
{
    bool highlight = IsPopupShown() && GetSelection() == item;

    dc.SetBrush(highlight ? *wxLIGHT_GREY_BRUSH : *wxWHITE_BRUSH);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(rect);
}


void MyComboBox::refresh()
{
    if (nGroups != mainFrame->reggy.getGroupCount()) {
        for (; nGroups < mainFrame->reggy.getGroupCount(); ++nGroups) {
            Append("Group #" + std::to_string(nGroups));
        }
        for (; nGroups > mainFrame->reggy.getGroupCount(); --nGroups) {
            Delete(nGroups - 1);
        }

        Select(0);
        mainFrame->reggy.setPriority(Reggy::NO_GROUP);
        SetString(0, wxString("Total Groups: " + std::to_string(nGroups)));
    }
}

void MyComboBox::OnComboBox(wxCommandEvent& event) 
{
    size_t group = (GetSelection() == wxNOT_FOUND || GetSelection() == 0) ? Reggy::NO_GROUP : (GetSelection() - 1);
    mainFrame->reggy.setPriority(group);
    mainFrame->refresh();
    event.Skip();
}

void MyComboBox::OnDrawItem(wxDC &dc, const wxRect &r, int item, int flags) const
{
    if (item == wxNOT_FOUND) return;

    wxPoint p = r.GetTopLeft() + wxPoint(4,4);

    if (item > 0) {
        wxRect rect = r;
        rect.SetTop(rect.GetTop() + 1);
        rect.SetBottom(rect.GetBottom() - 2);
        rect.SetRight(rect.GetLeft() + 48);
    
        wxColor bgColor = item > 0 ? mainFrame->colors[item-1] : GetBackgroundColour();
        dc.SetBrush(wxBrush(bgColor));
        dc.SetPen(wxPen(*wxBLACK));
        dc.DrawRectangle(rect);
        p += wxPoint(48, 0);
    }
    dc.SetTextForeground(*wxBLACK);
    dc.DrawText(GetString(item), p);
}

wxCoord MyComboBox::OnMeasureItem(size_t n) const
{
    return 26;
}

void MyComboBox::OnDropdown(wxCommandEvent& event)
{
    wxSetCursor(event.GetEventType() == wxEVT_COMBOBOX_DROPDOWN ? wxCURSOR_HAND : wxNullCursor);
    event.Skip();
}

// ----- MyFrame ---------

MyFrame::MyFrame() : isRefreshing(false), reggy(0),
  wxFrame(nullptr, wxID_ANY, "Reggy, the POSIX BRE/ERE Companion", wxPoint(100,100), wxSize(800, 600))
{
    colors.push_back(*wxLIGHT_GREY);
    colors.push_back(*wxRED);
    colors.push_back(*wxBLUE);
    colors.push_back(*wxGREEN);

    // Create sizers to arrange the controls
    wxPanel *ctrlPanel          = new wxPanel(this);
    wxBoxSizer *ctrlSizer       = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *opSizer         = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *flagSizer       = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *mainSizer       = new wxBoxSizer(wxVERTICAL);

    // Create controls and inputs
    inpPattern  = new wxTextCtrl(ctrlPanel, ID_INP_PTRN, wxEmptyString);
    inpData     = new wxRichTextCtrl(this, ID_INP_DATA, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER  | wxTE_MULTILINE);
    btnLoad     = new wxButton(ctrlPanel, ID_BTN_LOAD, wxString("Load"));
    btnSave     = new wxButton(ctrlPanel, ID_BTN_SAVE, wxString("Save"));
    btnQuit     = new wxButton(ctrlPanel, ID_BTN_QUIT, wxString("Quit"));
    btnClear    = new wxButton(ctrlPanel, ID_BTN_CLEAR, wxString("Clear"));
    cbMultiline = new wxCheckBox(ctrlPanel, ID_CB_MULTI, wxString("Multiline"));
    cbERE       = new wxCheckBox(ctrlPanel, ID_CB_FL_ERE, wxString("REG_EXTENDED"));
    cbICase     = new wxCheckBox(ctrlPanel, ID_CB_FL_ICASE, wxString("REG_ICASE"));
    cbNL        = new wxCheckBox(ctrlPanel, ID_CB_FL_NL, wxString("REG_NEWLINE"));
    comGroups   = new MyComboBox(ctrlPanel, ID_CO_GROUPS, this);
    cbNL->Disable();

    // Structure the layout with the sizers
    opSizer->Add(btnLoad, 0, wxCENTER | wxALL, 5);
    opSizer->Add(btnSave, 0, wxCENTER | wxALL, 5);
    opSizer->Add(comGroups, 1, wxCENTER | wxALL, 5);
    opSizer->Add(btnClear, 0, wxCENTER | wxALL, 5);
    opSizer->Add(btnQuit, 0, wxCENTER | wxALL, 5);

    flagSizer->Add(cbMultiline, 0, wxCENTER | wxALL, 5);
    flagSizer->AddStretchSpacer(1);
    flagSizer->Add(cbERE, 0, wxCENTER | wxALL, 5);
    flagSizer->Add(cbICase, 0, wxCENTER | wxALL, 5);
    flagSizer->Add(cbNL, 0, wxCENTER | wxALL, 5);

    wxStaticBoxSizer *box = new wxStaticBoxSizer(wxHORIZONTAL, ctrlPanel, "Regex Pattern");
    box->Add(inpPattern, 1, wxEXPAND | wxALL, 5);
    ctrlSizer->Add(box, 1, wxEXPAND | wxALL, 5);
    ctrlSizer->Add(opSizer, 0, wxEXPAND | wxALL, 5);
    ctrlSizer->Add(flagSizer, 0, wxEXPAND | wxALL, 5);
    ctrlPanel->SetSizer(ctrlSizer);

    mainSizer->Add(ctrlPanel, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(inpData, 1, wxEXPAND);

    SetSizer(mainSizer);
    SetAutoLayout(true);
    SetMinSize(wxSize(700, 500));
    CreateStatusBar(1);
}

MyFrame::~MyFrame() {}

void MyFrame::refresh()
{
    SetStatusText(!reggy.isReady() ? wxString(reggy.getErrorString()) : wxT(""), 0);

    isRefreshing = true;
    auto cursor = inpData->GetCaretPosition();
    wxString s = inpData->GetValue();
    inpData->Clear();
    inpData->SetDefaultStyle(wxRichTextAttr());
    if (s.empty()) {
        // do nothing
    } else if (!reggy.isReady()) {
        inpData->WriteText(s);
    } else {
        if (colors.size() < reggy.getGroupCount()) {
            for (auto i = colors.size(); i < reggy.getGroupCount(); ++i) {
                colors.push_back(wxColour(rand() % 256, rand() % 256, rand() % 256));
            }
        }

        size_t pos = 0, i = 0;
        while (pos < s.size()) {
            wxRichTextAttr attr;
            size_t group = reggy.getPrimaryGroup(i);
            if (group != Reggy::NO_GROUP) {
                float avg = ((float)colors[group].Red() + colors[group].Green() + colors[group].Blue()) / 3.0f;
                attr.SetTextColour(avg < 127.5f ? *wxWHITE : *wxBLACK);
                attr.SetBackgroundColour(colors[group]);
            }
            inpData->BeginStyle(attr);
            inpData->WriteText(s.substr(pos, reggy.getStopLen(i)));
            inpData->EndStyle();
            pos += reggy.getStopLen(i++);
        }
    }
    comGroups->refresh();
    inpData->SetCaretPosition(cursor);
    isRefreshing = false;
}

void MyFrame::OnChangePtrn(wxCommandEvent& event)
{
    reggy.setPattern(std::string(inpPattern->GetValue()));
    refresh();
    event.Skip();
}

void MyFrame::OnChangeData(wxCommandEvent& event)
{
    if (!isRefreshing) {
        reggy.setData(std::string(inpData->GetValue()));
        refresh();
    }
    event.Skip();
}

void MyFrame::OnBtnClear(wxCommandEvent& event)
{
    inpPattern->SetValue(wxEmptyString);
    inpData->SetValue(wxEmptyString);
    comGroups->Select(0);
    event.Skip();
}

void MyFrame::OnBtnLoad(wxCommandEvent& event)
{
    event.Skip();

    wxFileDialog loadFile(this, "Load File", wxEmptyString, wxEmptyString, "JSON files (*.json)|*.json", wxFD_FILE_MUST_EXIST | wxFD_OPEN);

    if (loadFile.ShowModal() == wxID_CANCEL) {
        return;
    }
    
    wxFile inputStream(loadFile.GetPath());

    wxString jsonSrc;
    inputStream.ReadAll(&jsonSrc);

    // C from here on:
    char error[1024];
    yajl_val node = yajl_tree_parse(jsonSrc.c_str(), error, sizeof(error));

    if (!node) {
        wxLogError("File format not recognized.");
        return;
    }
    yajl_val result = yajl_tree_get(node, (const char *[]){"pattern", (const char *)0}, yajl_t_string);
    if (result) {
        inpPattern->SetValue(wxString(YAJL_GET_STRING(result)));
    } else {
        wxLogDebug("No pattern found.");
    }

    result = yajl_tree_get(node, (const char *[]){"data", (const char *)0}, yajl_t_string);
    if (result) {
        inpData->SetValue(wxString(YAJL_GET_STRING(result)));
    } else {
        wxLogDebug("No data found.");
    }

    yajl_tree_free(node);
}

void MyFrame::OnBtnSave(wxCommandEvent& event)
{
    event.Skip();

    wxFileDialog saveFile(this, "Save File", wxEmptyString, wxEmptyString, "JSON files (*.json)|*.json", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (saveFile.ShowModal() == wxID_CANCEL) {
        return;
    }
    
    wxFile outputStream(saveFile.GetPath(), wxFile::write);

    yajl_gen yajl = NULL;
    if (!(yajl = yajl_gen_alloc(NULL))) {
        wxLogError("YAJL error, could not init.");
        return;
    }

    yajl_gen_map_open(yajl);
    if (!yajl) {
        wxLogError("YAJL error, could not open map.");
        return;
    }

    auto my_yajl = [&](const char s[]) {
        return yajl_gen_string(yajl, (const unsigned char *)s, strlen(s));        
    };

    if (my_yajl("pattern") || my_yajl(inpPattern->GetValue().c_str())) {
        wxLogError("YAJL error, could not write pattern.");
    }

    if (my_yajl("data") || my_yajl(inpData->GetValue().c_str())) {
        wxLogError("YAJL error, could not write data.");
    }

    yajl_gen_map_close(yajl);
    const unsigned char *buf;
    size_t bufsize;
    yajl_gen_get_buf(yajl, &buf, &bufsize);

    outputStream.Write(wxString(buf));

    yajl_gen_free(yajl);
}

void MyFrame::OnBtnQuit(wxCommandEvent& event)
{
    event.Skip();
    Destroy();
}

void MyFrame::OnCheckbox(wxCommandEvent& event)
{
    switch (event.GetId()) {
        case    ID_CB_FL_ERE : reggy.setFlag(REG_EXTENDED, cbERE->GetValue()); break;
        case  ID_CB_FL_ICASE : reggy.setFlag(REG_ICASE, cbICase->GetValue()); break;
        case     ID_CB_FL_NL : reggy.setFlag(REG_NEWLINE, cbNL->GetValue()); break;
        case     ID_CB_MULTI :
            reggy.setMultiline(cbMultiline->GetValue());
            if (cbMultiline->GetValue())
                cbNL->Enable();
            else
                cbNL->Disable();
            refresh();
            break;
    }
    refresh();
    event.Skip();
}

// ----- MyApp ---------

MyApp::MyApp() : myFrame(nullptr) {}

MyApp::~MyApp() {}

bool MyApp::OnInit()
{
    myFrame = new MyFrame();
    myFrame->Show();

    return true;
}