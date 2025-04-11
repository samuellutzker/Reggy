#pragma once
#include <wx/wx.h>
#include <wx/file.h>
#include <wx/vector.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/odcombo.h>
#include <wx/clipbrd.h>
#include <yajl/yajl_tree.h>
#include <yajl/yajl_gen.h>
#include "reggy.h"
#include <string>

#define ID_INP_PTRN     10001
#define ID_INP_DATA     10002
#define ID_BTN_CLEAR    10003
#define ID_CB_MULTI     10004
#define ID_CB_FL_ERE    10005
#define ID_CB_FL_ICASE  10006
#define ID_CB_FL_NL     10007
#define ID_CO_GROUPS    10008
#define ID_BTN_SAVE     10009
#define ID_BTN_LOAD     10010
#define ID_BTN_QUIT     10011
#define ID_BTN_COPY     10012

class MyComboBox;
class MyFrame;

class MyComboPopup : public wxVListBoxComboPopup {
    MyComboBox *parent;
    void OnMotion(wxMouseEvent &event);
    void OnMouseLeave(wxMouseEvent &event);
public:
    inline static constexpr int ITEM_HEIGHT = 26;
    MyComboPopup(MyComboBox *parent);

    wxDECLARE_EVENT_TABLE();
};

class MyComboBox : public wxOwnerDrawnComboBox {
    MyFrame *mainFrame;
    size_t nGroups;
    size_t hover;
    size_t selected;

    void OnDrawItem (wxDC &dc, const wxRect &rect, int item, int flags) const override;
    void OnDrawBackground(wxDC &dc, const wxRect &rect, int item, int flags) const override;
    void OnComboBox(wxCommandEvent &event);
    void OnDropdown(wxCommandEvent &event);
    wxCoord OnMeasureItem(size_t n) const override;

public:
    MyComboBox(wxWindow *parent, wxWindowID id, MyFrame *mainFrame);
    ~MyComboBox(void);
    void update(void);
    void setHover(size_t n);

    wxDECLARE_EVENT_TABLE();
};

class MyFrame : public wxFrame {
    friend class MyComboBox;
    wxTextCtrl *inpPattern;
    wxRichTextCtrl *inpData;
    wxButton *btnClear, *btnLoad, *btnSave, *btnQuit, *btnCopy;
    wxCheckBox *cbMultiline, *cbERE, *cbNLine, *cbICase;
    wxVector<wxColor> colors;
    Reggy reggy;
    bool isUpdating;

    void OnBtnClear(wxCommandEvent &event);
    void OnBtnLoad(wxCommandEvent &event);
    void OnBtnSave(wxCommandEvent &event);
    void OnBtnQuit(wxCommandEvent &event);
    void OnBtnCopy(wxCommandEvent &event);
    void OnChangePtrn(wxCommandEvent &event);
    void OnChangeData(wxCommandEvent &event);
    void OnCheckbox(wxCommandEvent &event);

public:
    MyComboBox *comGroups;
    MyFrame(void);
    ~MyFrame(void);
    void update(void);

    wxDECLARE_EVENT_TABLE();
};

class MyApp : public wxApp {
    MyFrame *myFrame;
public:
    MyApp(void);
    ~MyApp(void);

    virtual bool OnInit(void) override;
    // int FilterEvent(wxEvent&) override;
};

