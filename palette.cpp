#include "palette.h"
#include "okcolor.h"

// ------------------------------------------------------------------
/* ***					***
 * *** Palette Renderer ***
 * ***					***
 */
void PaletteRenderer::Draw(wxGrid& grid, wxGridCellAttr& attr, wxDC& dc, const wxRect& rect, int row, int col, bool isSelected) {
	// Render the data as a colour for any given cell of the grid
	dc.SetBackgroundMode(wxBRUSHSTYLE_SOLID);

	wxColour clr;
	if (grid.IsThisEnabled()) {
		if (isSelected) {
			if (grid.HasFocus()) {
				clr = grid.GetSelectionBackground();

			} else {
				clr = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW);
			}
		
		} else {
			wxStringTokenizer colourTokenizer(grid.GetCellValue(row, col), ",");

			wxString inputRed   = colourTokenizer.GetNextToken();
			wxString inputGreen = colourTokenizer.GetNextToken();
			wxString inputBlue  = colourTokenizer.GetNextToken();

			int red;
			int green;
			int blue;

			sscanf(inputRed.c_str(),   "%2X", &red);
			sscanf(inputGreen.c_str(), "%2X", &green);
			sscanf(inputBlue.c_str(),  "%2X", &blue);

			clr = wxColour(red, green, blue);
		}
	
	} else {
		// grey out fields if the grid is disabled
		clr = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
	}

	dc.SetBrush(clr);
	dc.SetPen(*wxTRANSPARENT_PEN);
	dc.DrawRectangle(rect);
}
// ------------------------------------------------------------------

// ------------------------------------------------------------------
/* ***				 ***
 * *** Palette Table ***
 * ***				 ***
 */
void PaletteTable::generatePalette() {
	// Before we resolve the palette, we want to blank it out
	for (int i = 0; i < _template->_components.size(); i++) {
		if (_template->_components[i]._setByUser == false) {
			_template->_components[i]._clr = wxColour(0,0,0,0);
		}
	}

	// First we find out how many groups we need
	int maxGroupNum = -1;
	for (int i = 0; i < _template->_components.size(); i++) {
		if (maxGroupNum < _template->_components[i]._group) {
			maxGroupNum = _template->_components[i]._group;
		}
	}
	std::vector<std::vector<Component *>> palette(maxGroupNum);

	// Next we add all the components to the groups
	for (int i = 0; i < _template->_components.size(); i++) {
		if (_template->_components[i]._group != -1) {
			palette[_template->_components[i]._group - 1].push_back(&_template->_components[i]);
		}
	}

	// Now we can resolve the palette in terms of groups
	resolvePalette(palette);

	// Now we resolve the constraints
	resolveBackgrounds(palette);

	unsigned char r;
	unsigned char g;
	unsigned char b;
	ok_color::RGB rgb;
	ok_color::HSV hsv;

	// For anything not part of a group, we want to resolve them individually
	for (int i = 0; i < _template->_components.size(); i++) {
		if ((_template->_components[i]._setByUser == false) && (_template->_components[i]._group == -1)) {
			wxColour clr;

			if (_template->_components[i]._reference != -1) {
				clr = _template->_components[_template->_components[i]._reference]._clr;

				rgb.r = clr.Red();
				rgb.g = clr.Green();
				rgb.b = clr.Blue();
				hsv = srgb_to_okhsv(rgb);
				
				if (std::isnan(hsv.h)) {
					hsv.h = 0;
				}

				if (std::isnan(hsv.s)) {
					hsv.s = 0;
				}

				if (std::isnan(hsv.v)) {
					hsv.v = 0;
				}

				switch (_template->_components[i]._relationC) {
					case kRelCCompliment:
						hsv.h = std::fmod((hsv.h + (180.0 / 360.0)), 1.0);
						break;
					case kRelCAnalogous:
						hsv.h = std::fmod((hsv.h + (50.0 / 360.0)), 1.0);
						break;
					case kRelCTriadic:
						hsv.h = std::fmod((hsv.h + (120.0 / 360.0)), 1.0);
						break;
					case kRelCTetradic:
						hsv.h = std::fmod((hsv.h + (90.0 / 360.0)), 1.0);
						break;
					case kRelCContrast:
						hsv.h = (1.0 - hsv.h);
						break;
					case kRelCNone:
						break;
					default:
						std::cout << "how did we get here??? C" << std::endl;
						break;
				}

				switch (_template->_components[i]._relationB) {
					case kRelBInvert:
						hsv.v = 94.277f - hsv.v;
						hsv.v = std::min(hsv.v + ((hsv.s * 100) / 2), 94.277f);
						break;
					case kRelBDarker:
						hsv.v = hsv.v / 2;
						break;
					case kRelBLighter:
						hsv.v = std::min(hsv.v * 2, 94.277f);
						break;
					case kRelBNone:
						break;
					default:
						std::cout << "how did we get here??? B" << std::endl;
						break;
				}

				switch (_template->_components[i]._relationS) {
					case kRelSInvert:
						hsv.s = (1.0 - hsv.s);
						break;
					case kRelSLower:
						hsv.s = hsv.s / 2;
						break;
					case kRelSHigher:
						hsv.s = std::min(hsv.s * 2, 1.0f);
						break;
					case kRelSNone:
						break;
					default:
						std::cout << "how did we get here??? S" << std::endl;
						break;
				}

				rgb = okhsv_to_srgb(hsv);
				r = rgb.r;
				g = rgb.g;
				b = rgb.b;
				clr = wxColour(r, g, b);
			
			} else {
				clr = wxColour(rand() % 255, rand() % 255, rand() % 255);
				
				rgb.r = clr.Red();
				rgb.g = clr.Green();
				rgb.b = clr.Blue();
				hsv = srgb_to_okhsv(rgb);

				if (std::isnan(hsv.h)) {
					hsv.h = 0;
				}

				if (std::isnan(hsv.s)) {
					hsv.s = 0;
				}

				if (std::isnan(hsv.v)) {
					hsv.v = 0;
				}

				if (_template->_components[i]._restrictBright == true) {
					hsv.v = std::min(std::get<1>(_template->_components[i]._rangeBright), hsv.v);
					hsv.v = std::max(std::get<0>(_template->_components[i]._rangeBright), hsv.v);
				}

				if (_template->_components[i]._restrictSat == true) {
					hsv.s = std::min(std::get<1>(_template->_components[i]._rangeSat), hsv.s);
					hsv.s = std::max(std::get<0>(_template->_components[i]._rangeSat), hsv.s);
				}

				rgb = okhsv_to_srgb(hsv);
				r = rgb.r;
				g = rgb.g;
				b = rgb.b;
				clr = wxColour(r, g, b);
			}

			_template->_components[i]._clr = clr;
		}
	}

	resolveConstraints();
}

void PaletteTable::resolvePalette(std::vector<std::vector<Component *>> palette) {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	ok_color::RGB rgb;
	ok_color::HSV hsv;

	wxColour dominantClr = wxColour(rand() % 255, rand() % 255, rand() % 255);

	// First step is to set a dominant colour and the adjacent ones for each group
	for (int p = 0; p < palette.size(); p++) {
		rgb.r = dominantClr.Red();
		rgb.g = dominantClr.Green();
		rgb.b = dominantClr.Blue();
		hsv = srgb_to_okhsv(rgb);

		hsv.h = std::fmod((hsv.h + (50.0 / 360.0)), 1.0);
		
		rgb = okhsv_to_srgb(hsv);
		r = rgb.r;
		g = rgb.g;
		b = rgb.b;
		dominantClr = wxColour(r, g, b);

		palette[p][0]->_clr = dominantClr;
	}

	for (int p = 0; p < palette.size(); p++) {
		rgb.r = palette[p][0]->_clr.Red();
		rgb.g = palette[p][0]->_clr.Green();
		rgb.b = palette[p][0]->_clr.Blue();
		hsv = srgb_to_okhsv(rgb);

		switch (palette[p].size()) {
			// 2 colours = complimentary
			case 2:
				hsv.h = std::fmod((hsv.h + (180.0 / 360.0)), 1.0);
				
				rgb = okhsv_to_srgb(hsv);
				r = rgb.r;
				g = rgb.g;
				b = rgb.b;

				palette[p][1]->_clr = wxColour(r, g, b);
				break;
			// 3 colours = triadic
			case 3:
				for (int i = 1; i < 3; i++) {
					hsv.h = std::fmod((hsv.h + (120.0 / 360.0)), 1.0);
					
					rgb = okhsv_to_srgb(hsv);
					r = rgb.r;
					g = rgb.g;
					b = rgb.b;

					palette[p][i]->_clr = wxColour(r, g, b);
				}
				break;
			// 4 colours = tetradic
			case 4:
				for (int i = 1; i < 4; i++) {
					hsv.h = std::fmod((hsv.h + (90.0 / 360.0)), 1.0);
					
					rgb = okhsv_to_srgb(hsv);
					r = rgb.r;
					g = rgb.g;
					b = rgb.b;

					palette[p][i]->_clr = wxColour(r, g, b);
				}
				break;
			// 5+ colours = not being dealt with yet
			default:
				std::cout << "how did we get here??? C" << std::endl;
				break;
		}

		for (int i = 0; i < palette[p].size(); i++) {
			rgb.r = palette[p][i]->_clr.Red();
			rgb.g = palette[p][i]->_clr.Green();
			rgb.b = palette[p][i]->_clr.Blue();
			hsv = srgb_to_okhsv(rgb);

			if (palette[p][i]->_restrictBright == true) {
				hsv.v = std::min(std::get<1>(palette[p][i]->_rangeBright), hsv.v);
				hsv.v = std::max(std::get<0>(palette[p][i]->_rangeBright), hsv.v);
			}

			if (palette[p][i]->_restrictSat == true) {
				hsv.s = std::min(std::get<1>(palette[p][i]->_rangeSat), hsv.s);
				hsv.s = std::max(std::get<0>(palette[p][i]->_rangeSat), hsv.s);
			}

			rgb = okhsv_to_srgb(hsv);
			r = rgb.r;
			g = rgb.g;
			b = rgb.b;
			palette[p][i]->_clr = wxColour(r, g, b);
		}
	}
}

void PaletteTable::resolveBackgrounds(std::vector<std::vector<Component *>> palette) {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	ok_color::RGB rgb;
	ok_color::HSV hsv;

	// Here we start by resolving the background vs foreground brightness problem
	for (int p = 0; p < palette.size(); p++) {
		int idxBack = -1;
		int idxFore = -1;
		for (int i = 0; i < palette[p].size(); i++) {
			if (palette[p][i]->_background == kBackground) {
				idxBack = i;
			
			} else if (palette[p][i]->_background == kForeground) {
				idxFore = i;
			}
		}

		// If we have found a background and a foreground, then we want to invert the brightness for them
		if ((idxBack != -1) && (idxFore != -1)) {
			wxColour backClr = palette[p][idxBack]->_clr;
			
			rgb.r = backClr.Red();
			rgb.g = backClr.Green();
			rgb.b = backClr.Blue();
			hsv = srgb_to_okhsv(rgb);

			float bright = hsv.v;
			float sat = hsv.s;

			wxColour foreClr = palette[p][idxFore]->_clr;
			
			rgb.r = foreClr.Red();
			rgb.g = foreClr.Green();
			rgb.b = foreClr.Blue();
			hsv = srgb_to_okhsv(rgb);

			hsv.v = 94.277f - bright;
			hsv.v = std::min(hsv.v + ((sat * 100) / 2), 94.277f);

			rgb = okhsv_to_srgb(hsv);
			r = rgb.r;
			g = rgb.g;
			b = rgb.b;

			palette[p][idxFore]->_clr = wxColour(r, g, b);
		}
	}
}

void PaletteTable::resolveConstraints() {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	ok_color::RGB rgb;
	ok_color::HSV hsv;

	for (int i = 0; i < _template->_components.size(); i++) {
	}
}

void PaletteTable::generateTheme(int theme) {
}

void PaletteTable::generateColourType(int colourType) {
	/* Pastel: The ideal range of Saturation for these colors is 14–21 and of brightness is 89–96.
	 * Jewel: The ideal range of Saturation for these colors is around 83–73 and of brightness is around 76–56.
	 */
	unsigned char r;
	unsigned char g;
	unsigned char b;
	ok_color::RGB rgb;
	ok_color::HSV hsv;

	if (colourType > 0) {
		for (int i = 0; i < _template->_components.size(); i++) {
			if (_template->_components[i]._setByUser == false) {
				wxColour clr = _template->_components[i]._clr;
				rgb.r = clr.Red();
				rgb.g = clr.Green();
				rgb.b = clr.Blue();
				hsv = srgb_to_okhsv(rgb);

				switch (colourType) {
					case kTypePastel:
						hsv.v = std::min(hsv.v * 2, 92.0f);
						hsv.s = 0.17f;
						break;
					case kTypeJewl:
						hsv.v = std::min(hsv.v, 66.0f);
						hsv.s = 0.78f;
						break;
					case kTypeFluorescent:
						hsv.v = std::min(hsv.v * 2, 94.277f);
						hsv.s = 1.0f;
						break;
					default:
						std::cout << "colour type wtf" << std::endl;
				}

				rgb = okhsv_to_srgb(hsv);
				r = rgb.r;
				g = rgb.g;
				b = rgb.b;
				clr = wxColour(r, g, b);

				_template->_components[i]._clr = clr;
			}
		}
	}
}

wxString PaletteTable::GetValue(int row, int col) {
	switch (col) {
		case 0:
			return _template->_components[row]._name;
		case 1:
			return wxString::Format("%02X,%02X,%02X", _template->_components[row]._clr.Red(), _template->_components[row]._clr.Green(), _template->_components[row]._clr.Blue());
		case 2:
			return wxString::Format("%02X%02X%02X", _template->_components[row]._clr.Red(), _template->_components[row]._clr.Green(), _template->_components[row]._clr.Blue());
		case 3:
			return wxString::Format("%d,%d,%d", _template->_components[row]._clr.Red(), _template->_components[row]._clr.Green(), _template->_components[row]._clr.Blue());
		default:
			return "";
			std::cout << "how did we get here" << std::endl;
	}
}

void PaletteTable::SetValue(int row, int col, const wxString &value) {
	if (col > 0) {
		int red;
		int green;
		int blue;

		if ((col == 1) || (col == 2)) {
				wxString inputRed   = value.Mid(0, 2);
				wxString inputGreen = value.Mid(2, 2);
				wxString inputBlue  = value.Mid(4, 2);

				sscanf(inputRed.c_str(),   "%2X", &red);
				sscanf(inputGreen.c_str(), "%2X", &green);
				sscanf(inputBlue.c_str(),  "%2X", &blue);

		} else if (col == 3) {
				wxStringTokenizer colourTokenizer(value, ",");

				wxString inputRed   = colourTokenizer.GetNextToken();
				wxString inputGreen = colourTokenizer.GetNextToken();
				wxString inputBlue  = colourTokenizer.GetNextToken();

				sscanf(inputRed.c_str(),   "%3d", &red);
				sscanf(inputGreen.c_str(), "%3d", &green);
				sscanf(inputBlue.c_str(),  "%3d", &blue);

				red = red % 256;
				green = green % 256;
				blue = blue % 256;
		}
		_template->_components[row]._clr = wxColour(red, green, blue);
		_template->_components[row]._setByUser = true;
		GetView()->ForceRefresh();
	}
}

wxColour PaletteTable::getColour(int row, int col) {
	wxStringTokenizer colourTokenizer(GetValue(row, col), ",");

	wxString inputRed   = colourTokenizer.GetNextToken();
	wxString inputGreen = colourTokenizer.GetNextToken();
	wxString inputBlue  = colourTokenizer.GetNextToken();

	int red;
	int green;
	int blue;

	sscanf(inputRed.c_str(),   "%2X", &red);
	sscanf(inputGreen.c_str(), "%2X", &green);
	sscanf(inputBlue.c_str(),  "%2X", &blue);

	return wxColour(red, green, blue);
}

void PaletteTable::setColour(int row, int col, wxColour clr) {
	SetValue(row, col, wxString::Format("%02X%02X%02X", clr.Red(), clr.Green(), clr.Blue()));
}
// ------------------------------------------------------------------