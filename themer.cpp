// Colour Efficient
#include "themer.h"

IMPLEMENT_APP(Themer)

// ------------------------------------------------------------------
/* ***          ***
 * *** App Init ***
 * ***          ***
 */
bool Themer::OnInit()
{
	// We will be using .png images for the program, so we need a handler for them
	wxImage::AddHandler(new wxPNGHandler);

	// Default size is 530x580
	int sizeX = 1030;
	int sizeY = 580;

	// Create the frame that will contain the program
	ThemerFrame *frame = new ThemerFrame(wxSize(sizeX, sizeY));
	frame->Show(true);

	// Rando needs to be resizable, but we don't want it going smaller than the default
	frame->SetMinSize(wxSize(sizeX, sizeY));

	// Finally, we're finished, run the program
	return true;
}
// ------------------------------------------------------------------

// ------------------------------------------------------------------
/* ***               ***
 * *** The App Frame ***
 * ***               ***
 */
void ThemerFrame::debug(wxString s) {
	std::cout << s << std::endl;
}

void ThemerFrame::debug(int i) {
	std::cout << i << std::endl;
}

ThemerFrame::ThemerFrame(wxSize s) : wxFrame(NULL, wxID_ANY, "Color Efficient", wxDefaultPosition, s) {
	// We're using random numbers in this, so we need to seed the randomizer
	srand(time(0));

	// So first we give it a menu
	wxMenuBar *menuBar = new wxMenuBar;				// Create a menu bar for our menus
	populateMenuBar(menuBar);
	SetMenuBar(menuBar);

	// Next we give it a toolbar
	_toolBar = new wxToolBar(this, ID_ToolBar);
	populateToolBar();
	SetToolBar(_toolBar);
	_toolBar->Realize();

	// After the toolbar is the main program area, which needs a general sizer to correctly size the panel when resizing
	_programSizer = new wxBoxSizer(wxVERTICAL);
	   _mainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, "Main Panel");
	populateTemplates();
	populateMainPanel();
	_programSizer->Add(_mainPanel, 1, wxGROW);

	// And now we need to apply the sizer to the frame
	_programSizer->Layout();
	SetSizer(_programSizer);
}
// ------------------------------------------------------------------

// ------------------------------------------------------------------
/* ****              ****
 * **** The Menu Bar ****
 * ****              ****
 */
void ThemerFrame::populateMenuBar(wxMenuBar *menuBar) {

	/* ---- File ----
	 * -Load Rom
	 */
	wxMenu *menuFile = new wxMenu;
	menuFile->Append(ID_MenuOpen, "&Open...\tCtrl-O", "Load a palette (.pal)");
	menuFile->Append(ID_MenuOpen, "&Save...\tCtrl-S", "Save current palette");
	/* -------------- */

	/* ---- Help ----
	 * -Credits
	 * -Contact
	 */
	wxMenu *menuHelp = new wxMenu;
	menuHelp->AppendSeparator();
	menuHelp->Append(ID_MenuContact, "&Contact", "Show contact information");
	menuHelp->Append(ID_MenuCredits, "&Credits", "Show credits");
	/* -------------- */

	/* ---- Rando ----
	 * (special cases)
	 * Quit
	 * About
	 * Preferences
	 */
	menuFile->Append(wxID_EXIT);
	menuHelp->Append(wxID_ABOUT);
	/* --------------- */
 
	// Append the menu items to the menu bar
	menuBar->Append(menuFile, "&File");
	menuBar->Append(menuHelp, "&Help");

	// And finally, we bind all the events to their respective functions
	Bind(wxEVT_MENU, &ThemerFrame::onOpen,    	 this, ID_MenuOpen);
	Bind(wxEVT_MENU, &ThemerFrame::onSave,    	 this, ID_MenuSave);
	Bind(wxEVT_MENU, &ThemerFrame::onContact,	 this, ID_MenuContact);
	Bind(wxEVT_MENU, &ThemerFrame::onCredits, 	 this, ID_MenuCredits);
	Bind(wxEVT_MENU, &ThemerFrame::onAbout,   	 this, wxID_ABOUT);
	Bind(wxEVT_MENU, &ThemerFrame::onExit,    	 this, wxID_EXIT);
}

void ThemerFrame::onOpen(wxCommandEvent& event) {
	// Prompt the user to open a rom file
	wxFileDialog open(this, _("Open palette file"), "", "", "Palette files (*.pal)|*.pal", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	// If they decide to cancel, just return
	if (open.ShowModal() == wxID_CANCEL) {
		return;
	}

	// Base path will be used later
	_basePath = open.GetPath();

	if (!wxFile::Exists(_basePath)) {
		wxLogError("Cannot open file");
		return;
	}
}

void ThemerFrame::onSave(wxCommandEvent& event) {
	wxFileDialog open(this, _("Choose file to save as"), "", "", "Palette files (*.pal)|*.pal", wxFD_SAVE);

	if (open.ShowModal() != wxID_CANCEL) {
		_outputPath = open.GetPath();
	}
}

void ThemerFrame::onContact(wxCommandEvent& event) {
	wxMessageBox("Discord: Quote58#6249\nTwitter: @Quote_58", "Please report issues to:", wxOK | wxICON_INFORMATION);
}
 
void ThemerFrame::onCredits(wxCommandEvent& event) {
	wxMessageBox("??","Special thanks to: People!", wxOK | wxICON_INFORMATION);
}

void ThemerFrame::onAbout(wxCommandEvent& event) {
	wxMessageBox("Colours!", "About Colour Bind", wxOK | wxICON_INFORMATION);
}

void ThemerFrame::onExit(wxCommandEvent& event) {
	// This true ensures that this close button has vito over all windows
	Close(true);
}
// ------------------------------------------------------------------

// ------------------------------------------------------------------
/* ****              ****
 * **** The Tool Bar ****
 * ****              ****
 */
void ThemerFrame::populateToolBar() {
	// The \t is to make the dropdown large enough to read be default
	wxString colourChoices[4] = {"None", "Pastel", "Jewl", "Fluorescent"};
	wxStaticText *colourText = new wxStaticText(_toolBar, wxID_ANY, "Colour Type ");
				 _colourChoice = new wxChoice(_toolBar, wxID_ANY, wxDefaultPosition, wxDefaultSize, 4, colourChoices, 0, wxDefaultValidator, wxEmptyString);

	wxString themeChoices[6] = {"None", "Halloween", "Christmas", "Easter", "New Years", "Thanksgiving"};
	wxStaticText *themeText = new wxStaticText(_toolBar, wxID_ANY, "Theme ");
				 _themeChoice = new wxChoice(_toolBar, wxID_ANY, wxDefaultPosition, wxDefaultSize, 6, themeChoices, 0, wxDefaultValidator, wxEmptyString);

	_toolBar->AddControl(colourText);
	_toolBar->AddControl(_colourChoice);
	_toolBar->AddControl(themeText);
	_toolBar->AddControl(_themeChoice);
}
// ------------------------------------------------------------------

// ------------------------------------------------------------------
/* ****                ****
 * **** The Main Panel ****
 * ****                ****
 */
void ThemerFrame::populateTemplates() {
	/* This is where we create the base templates (ie. templates without colour data)
	 * which are then loaded into the individual palette grids
	 */
	Template *templateWebsite = new Template();
	templateWebsite->_imageBytes = _imageBytesWebsite;
	templateWebsite->_imageSize = 25680;
	templateWebsite->_imagePalette = {wxColour(252,241,113), wxColour(226,121,46), wxColour(164,247,106), wxColour(241,155,145), wxColour(237,110,87), wxColour(218,59,38), wxColour(28,70,114), wxColour(241,154,200), wxColour(137,32,82)};
	templateWebsite->_components = {
	Component("Header Background", 1, kBackground),
	Component("Header Forground", 1, kForeground),
	Component("Background", 2, kBackground),
	Component("Text [H1]", 2, kForeground),
	Component("Text [H2]", -1, 3, kRelCNone, kRelBDarker, kRelSNone, kBackNone),
	Component("Text [H3]", -1, 4, kRelCNone, kRelBDarker, kRelSNone, kBackNone),
	Component("Accent Background", 3, kBackground),
	Component("Accent Foreground", 3, kForeground),
	Component("Footer Background", 4, kBackground),
	Component("Footer Foreground", 4, kForeground)
	};
	_initTemplates.push_back(templateWebsite);

	Template *templateIcon = new Template();
	templateIcon->_imageBytes = _imageBytesIcon;
	templateIcon->_imageSize = 3761;
	templateIcon->_imagePalette = {wxColour(0,0,0,0)};
	templateIcon->_components = {
	Component("Background", 1, kBackground),
	Component("Foreground", 1, kForeground),
	Component("Text", 1, kBackNone)
	};
	_initTemplates.push_back(templateIcon);

	Template *templateText = new Template();
	templateText->_imageBytes = _imageBytesText;
	templateText->_imageSize = 27657;
	templateText->_imagePalette = {wxColour(255,255,255), wxColour(255,5,0), wxColour(0,255,33), wxColour(255,241,1), wxColour(0,37,255)};
	templateText->_components = {
	Component("Background", 1, true, std::make_tuple(0, 60), true, std::make_tuple(0,70), kBackground),
	Component("Text", 1, kForeground),
	Component("Outline", -1, 1, kRelCNone, kRelBDarker, kRelSLower, kBackNone),
	Component("Drop Shadow", -1, 2, kRelCNone, kRelBDarker, kRelSLower, kBackNone),
	Component("Subtitle", -1, 1, kRelCNone, kRelBDarker, kRelSNone, kBackNone),
	Component("Glow", -1, 1, kRelCAnalogous, kRelBLighter, kRelSHigher, kBackNone)
	};
	_initTemplates.push_back(templateText);
}

void ThemerFrame::populateMainPanel() {
	// We start with a sizer to contain both the palette and preview boxes
	wxBoxSizer *templateSizer = new wxBoxSizer(wxHORIZONTAL);

	// The palette box being first
	_paletteBox = new wxStaticBoxSizer(wxVERTICAL, _mainPanel, wxEmptyString);

	// Within the palette box we need a sizer for the buttons across the top
	wxBoxSizer *topSizer = new wxBoxSizer(wxHORIZONTAL);
	
	// The top sizer contains the template chooser and the reset/generate buttons
	// First the template chooser
	wxString templateChoices[3] = {"Website", "Icon", "Text"};
	wxStaticText *templateText = new wxStaticText(_paletteBox->GetStaticBox(), wxID_ANY, "Template ");
	wxChoice *templateChoice = new wxChoice(_paletteBox->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 3, templateChoices, 0, wxDefaultValidator, wxEmptyString);
	templateChoice->Bind(wxEVT_CHOICE, &ThemerFrame::onTemplate, this);

	// Then the reset button
	wxButton *resetButton = new wxButton(_paletteBox->GetStaticBox(), wxID_ANY, "Reset", wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxEmptyString);
	resetButton->Bind(wxEVT_BUTTON, &ThemerFrame::onReset, this);

	// And the generate button
	wxButton *generateButton = new wxButton(_paletteBox->GetStaticBox(), wxID_ANY, "Generate", wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxEmptyString);
	generateButton->Bind(wxEVT_BUTTON, &ThemerFrame::onGenerate, this);

	// Now we add all of them to the top sizer
	topSizer->Add(templateText, 0, wxALIGN_CENTER_VERTICAL);
	topSizer->Add(templateChoice);
	topSizer->AddStretchSpacer();
	topSizer->Add(resetButton);
	topSizer->Add(generateButton, 0, wxLEFT, 5);

	// And the topsizer into the palette box
	_paletteBox->Add(topSizer, 0, wxGROW);

	// Next are the palette grids, which are all added to the sizer, but only one is visible at once
	for (int i = 0; i < _initTemplates.size(); i++) {
		PaletteTable *table = new PaletteTable(_initTemplates[i]);
		_paletteGrids.push_back(createPaletteGrid(table));
		_paletteGrids[i]->Show(false);
		_paletteBox->Add(_paletteGrids[i], 1, wxGROW | wxTOP, 5);
	}
	_paletteGrids[_template]->Show(true);

	// Now we create the preview panel with it's image panels
	_previewBox = new wxStaticBoxSizer(wxVERTICAL, _mainPanel, wxEmptyString);
	wxStaticText *previewText = new wxStaticText(_previewBox->GetStaticBox(), wxID_ANY, "Preview");
	_previewBox->Add(previewText, 0, wxGROW);
	_previewBox->Add(new wxStaticLine(_previewBox->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxSize(0,12), wxLI_HORIZONTAL, wxEmptyString), 0, wxGROW);	

	for (int i = 0; i < _initTemplates.size(); i++) {
		PaletteTable *table = (PaletteTable *) _paletteGrids[i]->GetTable();
		ImagePanel *previewImage = new ImagePanel((wxPanel *) _previewBox->GetStaticBox(), table->_template->_imageBytes, table->_template->_imageSize);
		previewImage->updatePalette(table->_template->_imagePalette, table->_template->_components);
		_previewImages.push_back(previewImage);
		_previewImages[i]->Show(false);
		_previewBox->Add(previewImage, 1, wxGROW | wxTOP, 20);
	}
	_previewImages[_template]->Show(true);


	// And finally we add all of the parts into the main sizer
	templateSizer->Add(_paletteBox, 0, wxGROW | wxLEFT | wxTOP | wxBOTTOM, kMacMargins);
	templateSizer->Add(_previewBox, 1, wxGROW | wxLEFT | wxTOP | wxRIGHT | wxBOTTOM, kMacMargins);
	_mainPanel->SetSizer(templateSizer);
}

void ThemerFrame::onTemplate(wxCommandEvent &event) {
	// We just want to hide the last grid and show the current one
	_paletteGrids[_template]->Show(false);
	_previewImages[_template]->Show(false);
	_template = event.GetSelection();
	_paletteGrids[_template]->Show(true);
	_previewImages[_template]->Show(true);

	// And to make the sizer work again we need to send a size event
	SendSizeEvent();
}

void ThemerFrame::onReset(wxCommandEvent &event) {
	// This literally just blanks out the colour in each component of the current palette table
	PaletteTable *table = (PaletteTable *) _paletteGrids[_template]->GetTable();
	for (int i = 0; i < table->_template->_components.size(); i++) {
		table->_template->_components[i]._clr = wxColour(0,0,0,0);
		table->_template->_components[i]._setByUser = false;
	}

	_previewImages[_template]->updatePalette(table->_template->_imagePalette, table->_template->_components);

	_paletteGrids[_template]->ForceRefresh();
	_previewImages[_template]->Refresh();
}

void ThemerFrame::onGenerate(wxCommandEvent &event) {
	// Here is where the magic actually happens
	PaletteTable *table = (PaletteTable *) _paletteGrids[_template]->GetTable();
	table->generatePalette();
	//table->generateTheme(_themeChoice->GetSelection());
	//table->generateColourType(_colourChoice->GetSelection());

	_previewImages[_template]->updatePalette(table->_template->_imagePalette, table->_template->_components);

	_paletteGrids[_template]->ForceRefresh();
	_previewImages[_template]->Refresh();
}

wxGrid *ThemerFrame::createPaletteGrid(PaletteTable *table) {
	// We start by making a grid with the table we created prior
	wxGrid *grid = new wxGrid(_paletteBox->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_DOUBLE, wxEmptyString);
	grid->SetTable(table, false);

	// Then we need to set some default settings for the grid
	grid->SetDefaultCellAlignment(wxALIGN_CENTER, wxALIGN_CENTER);
	grid->SetRowLabelSize(0);
	grid->SetColLabelSize(0);
	grid->DisableDragColMove();
	grid->DisableDragRowMove();
	grid->DisableDragColSize();
	grid->DisableDragGridSize();
	grid->DisableDragRowSize();
	grid->EnableGridLines(true);

	// Why doesn't this work??
	grid->SetDefaultCellBackgroundColour(wxColour(0,0,0,0));

	// We need to set the second column as having a palette renderer instead of string
	wxGridCellAttr *cellAttr = grid->GetOrCreateCellAttr(0, 1);
	wxGridCellRenderer *renderer = new PaletteRenderer();
	cellAttr->SetRenderer(renderer);
	grid->SetColAttr(1, cellAttr);

	// We also want to change the font size to be larger
	wxFont f = grid->GetDefaultCellFont();
	f.Scale(1.5);
	grid->SetDefaultCellFont(f);

	// The size of the column should be the size of the largest string in the component names
	int num = 0;
	for (int i = 0; i < table->_template->_components.size(); i++) {
		if (table->_template->_components[i]._name.size() > table->_template->_components[num]._name.size()) {
			num = i;
		}
	}

	// Now that we know which string that is, we can size all the columns
	grid->SetColSize(0, grid->GetTextExtent(table->_template->_components[num]._name).GetWidth() * 2);
	grid->SetColSize(1, grid->GetTextExtent("COLOUR").GetWidth() * 2);
	grid->SetColSize(2, grid->GetTextExtent("0FFFFFF0").GetWidth() * 2);
	grid->SetColSize(3, grid->GetTextExtent("255,255,255").GetWidth() * 2);

	// By default the row size is 2x the default, because the text is 1.5x the default
	grid->SetDefaultRowSize(grid->GetDefaultRowSize() * 2);

	// Scrolling should be based on the size of a row
	grid->SetScrollLineY(grid->GetDefaultRowSize());

	// Now refresh the grid (probably not needed)
	grid->ForceRefresh();

	// And finally we bind the click action to a function so we can handle clicking on the colour
	grid->Bind(wxEVT_GRID_CELL_LEFT_CLICK, &ThemerFrame::onPaletteClick, this);

	return grid;
}

void ThemerFrame::onPaletteClick(wxGridEvent &event) {
	int row = event.GetRow();
	int col = event.GetCol();

	if (col == 1) {
		wxGrid *grid = (wxGrid *) event.GetEventObject();
		PaletteTable *gridTable = (PaletteTable *) grid->GetTable();

		// First we need the colour from the grid
		wxColour colour = gridTable->getColour(row, col);

		// To start the dialog with a certain colour, we need to put it into a colourData object
		wxColourData *cellClrData = new wxColourData();
		cellClrData->SetColour(colour);

		// Show the colour dialog with the cell colour
		wxColourDialog *clrDialog = new wxColourDialog(_mainPanel, cellClrData);
		clrDialog->ShowModal();

		// Get the current colour data from the dialog
		wxColourData clrData = clrDialog->GetColourData();
		wxColour newClr = clrData.GetColour();

		// Now that we have our colour, we can delete the dialog
		clrDialog->Destroy();

		gridTable->setColour(row, col, newClr);

		event.Skip();
	
	} else {
		event.Skip();
	}
}

// ------------------------------------------------------------------





























