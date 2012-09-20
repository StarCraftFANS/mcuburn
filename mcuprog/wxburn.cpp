#include "wx/wxprec.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif							/*  */

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif							/*  */
#include "wx/dynarray.h"
#include "wx/progdlg.h"
// define this to use wxExecute in the exec tests, otherwise just use system
#define USE_EXECUTE
#ifdef USE_EXECUTE
#define EXEC(cmd) wxExecute((cmd), wxEXEC_SYNC)
#else							/*  */
#define EXEC(cmd) system(cmd)
#endif							/*  */
#include "burn.xpm"
#include "mcuburn.xpm"
#include "mcupanel.h"


static const wxString strcpuname[] = {
                    wxString("Generic"),
                    wxString("BootLoader based"),
                    wxString("JTAG based"),     
                    wxString("AT89c51/52/55/55WD"),
                    wxString("AT89c5131/AT89c5131"),
                    wxString("PIC 16F8xx"),
                    wxString("Philips LPC21xx"),
                    wxString("ADI ADuc21xx"),
                    wxString("ADI ADuc8xx")
};
static const wxString strhexname[] = {
                    wxString("Intex Hex/ihx"),
                    wxString("Montorola srecord"),
                    wxString("Binary Images")
};
static const wxString strprogname[] = {
                    wxString("None"),
                    wxString("Voltage Level"),
                    wxString("Switch Position"),
                    wxString("Address")
};
static const wxString strportname[] = {
                    wxString("Serial"),
                    wxString("Parallel"),
                    wxString("USB"),     
                    wxString("Network"),
                    wxString("Bluetooth"),
                    wxString("Wi-Fi"),
                    wxString("Modem"),
};
// Define a new application type
class wxBurnApp:public wxApp {

  public:
	wxBurnApp();
	virtual ~ wxBurnApp() {
	};
	virtual bool OnInit();

  public:
	wxCriticalSection m_critsect;
	wxSemaphore m_semAllDone;
	bool m_waitingUntilAllDone;
};


// Create a new application object
IMPLEMENT_APP(wxBurnApp)
// Define a new frame type
	class wxMCUControlObj:public wxFrame {
	  public:
		// ctor
		wxMCUControlObj(wxFrame * frame, const wxString & title, int x, int y,
						int w, int h);
		  virtual ~ wxMCUControlObj();
            void CreateMCUPanel(wxMCUPanel* panel,wxBoxSizer *sizer,wxSize & btnsize);
            void CreateActionPanel(wxMCUPanel *btnpanel,wxBoxSizer *sizer,wxSize & btnsize);
            wxMCUPanel  *m_panel;
            wxButton    *m_btnquit;   
            wxButton    *m_btnread;   
            wxButton    *m_btnwrite;
            wxButton         *m_btnverify;
            wxButton         *m_btnerase;
            wxButton         *m_btndis;
            wxButton         *m_btnsave;
            wxRadioButton    *m_rdsave;
            wxRadioButton    *m_rdlock;
            wxRadioButton    *m_rdverify;
            
            wxComboBox       *m_cmbcpus;
            wxComboBox       *m_cmbhex;
            wxComboBox       *m_cmbport;
            wxButton         *m_cmdport;
            wxComboBox       *m_cmbprog;
            wxButton         *m_cmdprog;
            wxStaticText          *m_gauge;
            wxTextCtrl            *m_txtconsole;
	  protected:
		// callbacks
		void OnQuit(wxCommandEvent & event);
		void OnClear(wxCommandEvent & event);
		void OnStartRead(wxCommandEvent & event);
		void OnShowMCU(wxCommandEvent & event);
		void OnAbout(wxCommandEvent & event);
		void OnIdle(wxIdleEvent & event);
		void OnStartWrite(wxCommandEvent & event);
		void OnDisAssemble(wxCommandEvent & event);
          
	  private:
		// the progress dialog which we show while worker thread is running
		wxProgressDialog *m_dlgProgress;
		bool m_cancelled;
		// protects m_cancelled
		wxCriticalSection m_critsectWork;
	DECLARE_EVENT_TABLE()};


// ID for the menu commands
enum { BURNER_QUIT = wxID_EXIT,
       BURNER_ABOUT = wxID_ABOUT,
       BURNER_TEXT = 101, 
       BURNER_CLEAR, 
       BURNER_START_READ = 201, 
       BURNER_START_WRITE,
       BURNER_START_DISASS,
       BURNER_START_SHOWMCU
};

        BEGIN_EVENT_TABLE(wxMCUControlObj, wxFrame) 
        EVT_MENU(BURNER_QUIT, wxMCUControlObj::OnQuit) 
        EVT_MENU(BURNER_CLEAR, wxMCUControlObj::OnClear)
        EVT_MENU(BURNER_START_READ, wxMCUControlObj::OnStartRead)
        EVT_MENU(BURNER_START_WRITE, wxMCUControlObj::OnStartWrite)
        EVT_MENU(BURNER_START_DISASS,wxMCUControlObj::OnDisAssemble)
        EVT_MENU(BURNER_START_SHOWMCU, wxMCUControlObj::OnShowMCU)
        EVT_MENU(BURNER_ABOUT, wxMCUControlObj::OnAbout)
        EVT_IDLE(wxMCUControlObj::OnIdle) 
        END_EVENT_TABLE()
        
 wxBurnApp::wxBurnApp():m_semAllDone
	()
{
	m_waitingUntilAllDone = false;
}


// `Main program' equivalent, creating windows and returning main app frame
bool wxBurnApp::OnInit()
{

	// uncomment this to get some debugging messages from the trace code
	// on the console (or just set WXTRACE env variable to include "thread")
	//wxLog::AddTraceMask("thread");

	// Create the main frame window
	wxMCUControlObj *frame =
		new wxMCUControlObj((wxFrame *) NULL, _T("MCU Burner"),
							50, 50,
							600,375);

	// Make a menubar
	wxMenuBar *menuBar = new wxMenuBar;
	wxMenu *menuFile = new wxMenu;

	menuFile->Append(BURNER_CLEAR, _T("&Clear log\tCtrl-L"));
	menuFile->AppendSeparator();
	menuFile->Append(BURNER_QUIT, _T("E&xit\tAlt-X"));
	menuBar->Append(menuFile, _T("&File"));
	wxMenu *menuThread = new wxMenu;

	menuThread->Append(BURNER_START_READ,
					   _T("&Read\tCtrl-N"));
	menuThread->Append(BURNER_START_WRITE,
					   _T("&Write"));
	menuThread->Append(BURNER_START_DISASS,
					   _T("DisAssenble\tCtrl-S"));
	menuThread->AppendSeparator();
	menuThread->Append(BURNER_START_SHOWMCU,
					   _T("&MCU list\tCtrl-P"));
	wxMenu *menuHelp = new wxMenu;

	menuHelp->Append(BURNER_ABOUT, _T("&About..."));
	menuBar->Append(menuHelp, _T("&Help"));
	frame->SetMenuBar(menuBar);

	// Show the frame
	frame->Show(true);
	SetTopWindow(frame);
	return true;
}


// My frame constructor
wxMCUControlObj::wxMCUControlObj(wxFrame * frame, const wxString & title, int x, int y, int w, int h):wxFrame(frame, wxID_ANY, title, wxPoint(x, y),
		wxSize(w,
			   h))
{
	SetIcon(wxIcon(burn_xpm));
	m_dlgProgress = (wxProgressDialog *) NULL;

#if wxUSE_STATUSBAR
	CreateStatusBar(2);

#endif // wxUSE_STATUSBAR
	//Creates the Status Bar
	SetStatusText(wxT("Loading All Settings....."));
	wxBoxSizer *topsizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *framesizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *panelsizer = new wxBoxSizer(wxHORIZONTAL);
	int width = GetClientRect().GetWidth();
	int hgt = GetClientRect().GetHeight();
	wxColour panelclr(50, 50, 100);
	wxSize panelsize(width,hgt);
     wxSize btnsize(width/5,40);
	m_panel =
             new wxMCUPanel((wxWindow *) this, -1, wxDefaultPosition, panelsize, panelclr);
	
     CreateActionPanel(m_panel,panelsizer,btnsize);
     CreateMCUPanel(m_panel,topsizer,btnsize);
     panelsizer->Add(topsizer,1,wxEXPAND);
     //panelsizer->Add(topsizer1,1,wxEXPAND);
     m_panel->SetSizer(panelsizer);
     framesizer->Add(m_panel,1,wxEXPAND);
	SetSizer(framesizer);
	SetAutoLayout(TRUE);
	SetStatusText(wxT("Done....."));
     
}

wxMCUControlObj::~wxMCUControlObj()
{

	// NB: although the OS will terminate all the threads anyhow when the main
	//     one exits, it's good practice to do it ourselves -- even if it's not
	//     completely trivial in this example

	// tell all the threads to terminate: note that they can't terminate while
	// we're deleting them because they will block in their OnExit() -- this is
	// important as otherwise we might access invalid array elements

	wxGetApp().m_critsect.Enter();

	wxGetApp().m_critsect.Leave();
}


void wxMCUControlObj::OnStartRead(wxCommandEvent & WXUNUSED(event))
{
        
}

void wxMCUControlObj::OnStartWrite(wxCommandEvent & WXUNUSED(event))
{


        
}
void wxMCUControlObj::OnDisAssemble(wxCommandEvent & WXUNUSED(event))
{


}
void wxMCUControlObj::OnShowMCU(wxCommandEvent & WXUNUSED(event))
{
        
}

// set the frame title indicating the current number of threads
void wxMCUControlObj::OnIdle(wxIdleEvent & event)
{

        
}

void wxMCUControlObj::OnQuit(wxCommandEvent & WXUNUSED(event))
{
	Close(true);
}



void wxMCUControlObj::OnAbout(wxCommandEvent & WXUNUSED(event))
{
	wxMessageDialog dialog(this,
						   _T
						   ("wxWidgets multithreaded application sample\n")
						   _T("(c) 1998 Julian Smart, Guilhem Lavaux\n")
						   _T("(c) 1999 Vadim Zeitlin\n")
						   _T("(c) 2000 Robert Roebling"),
						   _T("About wxThread sample"),
						   wxOK | wxICON_INFORMATION);
	dialog.ShowModal();
}

void wxMCUControlObj::OnClear(wxCommandEvent & WXUNUSED(event))
{
}
void wxMCUControlObj::CreateActionPanel(wxMCUPanel *btnpanel,wxBoxSizer *sizer,wxSize & btnsize)
{
if( !btnpanel || !sizer) 
        return;
     wxSizer *btnsizer = new wxBoxSizer(wxVERTICAL);
 m_btnquit=new wxButton(btnpanel, BURNER_QUIT,
                 _T("E&xit"),wxDefaultPosition,btnsize);

 m_btnread=new wxButton(btnpanel, BURNER_START_READ,
                 _T("Read"),wxDefaultPosition,btnsize);
 
     m_btnwrite=new wxButton(btnpanel,BURNER_START_WRITE,
                     _T("Write"),wxDefaultPosition,btnsize);
     m_btnerase=new wxButton(btnpanel,BURNER_START_WRITE,
                     _T("Erase"),wxDefaultPosition,btnsize);
     m_btnsave=new wxButton(btnpanel,BURNER_START_WRITE,
                     _T("Save"),wxDefaultPosition,btnsize);
     m_btnverify=new wxButton(btnpanel,BURNER_START_WRITE, 
                     _T("Verify"),wxDefaultPosition,btnsize);
     m_btndis=new wxButton(btnpanel,BURNER_START_WRITE,
                     _T("Show Code"),wxDefaultPosition,btnsize);
     
     wxBitmapButton *m_btnlogo=new wxBitmapButton(btnpanel,wxID_ANY,wxBitmap(mcuburn_xpm),wxDefaultPosition,btnsize);
     wxColour btnfgclr(*wxBLACK);
     wxColour btnbgclr(*wxWHITE);
     wxColour bgclr(255,200,150);
     m_btnquit->SetForegroundColour(btnfgclr);
     m_btnquit->SetBackgroundColour(btnbgclr);
     m_btnread->SetForegroundColour(btnfgclr);
     m_btnread->SetBackgroundColour(btnbgclr);
     m_btnwrite->SetForegroundColour(btnfgclr);
     m_btnwrite->SetBackgroundColour(btnbgclr);
     m_btnerase->SetForegroundColour(btnfgclr);
     m_btnerase->SetBackgroundColour(btnbgclr);
     m_btnsave->SetForegroundColour(btnfgclr);
     m_btnsave->SetBackgroundColour(btnbgclr);
     m_btnverify->SetForegroundColour(btnfgclr);
     m_btnverify->SetBackgroundColour(btnbgclr);
     m_btndis->SetForegroundColour(btnfgclr);
     m_btndis->SetBackgroundColour(btnbgclr);
     m_btnlogo->SetForegroundColour(btnfgclr);
     m_btnlogo->SetBackgroundColour(bgclr);
     
     btnsizer->Add(m_btnquit,0,wxGROW|wxALL,2);
     btnsizer->Add(m_btnread,0,wxGROW|wxALL,2);
     btnsizer->Add(m_btnwrite,0,wxGROW|wxALL,2);
     btnsizer->Add(m_btnerase,0,wxGROW|wxALL,2);
     btnsizer->Add(m_btnsave,0,wxGROW|wxALL,2);
     btnsizer->Add(m_btnverify,0,wxGROW|wxALL,2);
     btnsizer->Add(m_btndis,0,wxGROW|wxALL,0);
     btnsizer->Add(m_btnlogo,1,wxEXPAND);
     sizer->Add(btnsizer,0,wxGROW|wxALL,5);
}
void wxMCUControlObj::CreateMCUPanel(wxMCUPanel *panel,wxBoxSizer *sizer,wxSize & btnsize)
{
    if(! panel || !sizer) 
            return;
     wxBoxSizer *mcuusizer  = new wxBoxSizer(wxHORIZONTAL);   
	wxBoxSizer *mcuvsizer1 = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *mcuvsizer2 = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *mcuvsizer3 = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *mculsizer = new wxBoxSizer(wxHORIZONTAL);
     
     m_rdlock=new wxRadioButton(panel,BURNER_START_READ, _T("Lock"),wxDefaultPosition);
     m_rdverify=new wxRadioButton(panel,BURNER_START_READ, _T("Fast Verify"),wxDefaultPosition);
     wxStaticText *m_stcpu=new wxStaticText(panel,wxID_ANY, _T("Select CPU"),wxDefaultPosition);
     m_cmbcpus=new wxComboBox(panel, BURNER_START_READ, strcpuname[0],
					   wxDefaultPosition,wxDefaultSize, 9,
					   (const wxString *) strcpuname,
					   wxCB_SIMPLE | wxCB_READONLY | wxCB_DROPDOWN);
     wxStaticText *m_sthex=new wxStaticText(panel,wxID_ANY, _T("Select Image Format"),wxDefaultPosition);
     m_cmbhex=new wxComboBox(panel, BURNER_START_READ, strhexname[0],
					   wxDefaultPosition,wxDefaultSize, 3,
					   (const wxString *) strhexname,
					   wxCB_SIMPLE | wxCB_READONLY | wxCB_DROPDOWN);
     wxStaticText *m_stport=new wxStaticText(panel,wxID_ANY, _T("Select Port"),wxDefaultPosition);
     m_cmbport=new wxComboBox(panel, BURNER_START_READ, strportname[0],
					   wxDefaultPosition,wxDefaultSize, 7,
					   (const wxString *) strportname,
					   wxCB_SIMPLE | wxCB_READONLY | wxCB_DROPDOWN);
     wxStaticText *m_stprog=new wxStaticText(panel,wxID_ANY, _T("Programmer Properties"),wxDefaultPosition);
     m_cmbprog=new wxComboBox(panel, BURNER_START_READ, strprogname[0],
					   wxDefaultPosition,wxDefaultSize, 5,
					   (const wxString *) strprogname,
					   wxCB_SIMPLE | wxCB_READONLY | wxCB_DROPDOWN);
     m_cmdport=new wxButton(panel,BURNER_START_WRITE,
                     _T("Configure Port"),wxDefaultPosition);
     m_cmdprog=new wxButton(panel,BURNER_START_WRITE,
                     _T("Change"),wxDefaultPosition);
     
    m_gauge = new wxStaticText(panel, BURNER_START_READ,
                    _T("00%"),wxDefaultPosition
                    );
      m_txtconsole = new wxTextCtrl(panel, wxID_ANY, wxEmptyString,
                             wxDefaultPosition,
                             wxDefaultSize,
                             wxTE_MULTILINE);
     wxColour btnfgclr(*wxBLACK);
     wxColour btnbgclr(*wxWHITE);
     m_cmdport->SetForegroundColour(btnfgclr);
     m_cmdport->SetBackgroundColour(btnbgclr);
     m_cmdprog->SetForegroundColour(btnfgclr);
     m_cmdprog->SetBackgroundColour(btnbgclr);
     wxColour clr(0,0,0);
     m_cmbcpus->SetForegroundColour(clr);
     m_cmbhex->SetForegroundColour(clr);
     m_cmbport->SetForegroundColour(clr);
     m_cmbprog->SetForegroundColour(clr);
     m_txtconsole->SetForegroundColour(clr);
     m_txtconsole->SetBackgroundColour(*wxWHITE);
     
     
     mcuvsizer1->Add(m_rdlock,0,wxGROW|wxALL,2);
     mcuvsizer1->Add(m_rdverify,0,wxGROW|wxALL,2);
    mcuvsizer1->Add(m_gauge, 0, wxCENTRE | wxALL,35);
    

     mcuvsizer2->Add(m_stcpu,0,wxGROW|wxALL,2);
     mcuvsizer2->Add(m_cmbcpus,0,wxGROW|wxALL,9);
     mcuvsizer2->Add(m_stport,0,wxGROW|wxALL,2);
     mcuvsizer2->Add(m_cmbport,0,wxGROW|wxALL,2);
     mcuvsizer2->Add(m_cmdport,0,wxGROW|wxALL,2);
     
     mcuvsizer3->Add(m_sthex,0,wxGROW|wxALL,2);
     mcuvsizer3->Add(m_cmbhex,0,wxGROW|wxALL,9);
     mcuvsizer3->Add(m_stprog,0,wxGROW|wxALL,2);
     mcuvsizer3->Add(m_cmbprog,0,wxGROW|wxALL,2);
     mcuvsizer3->Add(m_cmdprog,0,wxGROW|wxALL,2);
     
     mculsizer->Add(m_txtconsole, 1, wxEXPAND);
     
     mcuusizer->Add(mcuvsizer1, 0, wxGROW|wxALL,2);
     mcuusizer->Add(mcuvsizer2,0,wxGROW|wxALL,2);     
     mcuusizer->Add(mcuvsizer3,0,wxGROW|wxALL,2);     
     sizer->Add(mcuusizer,0,wxGROW|wxALL,2);     
     sizer->Add(mculsizer,1,wxEXPAND);     
     m_gauge->Hide();
}
