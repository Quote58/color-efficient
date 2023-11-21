// Palette related stuff

#ifndef PALETTE_H
#define PALETTE_H

// For compilers that don't support precompilation, include "wx/wx.h"
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include <algorithm>
#include <wx/tokenzr.h>
#include "wx/grid.h"
#include "wx/generic/gridctrl.h"
#include "wx/generic/grideditors.h"

// Colour types
enum ColourType {
	kTypeNone,
	kTypePastel,
	kTypeJewl,
	kTypeFluorescent
};

// Other
enum Background {
	kBackNone,
	kBackground,
	kForeground
};

// Relationship between component colours
enum RelationshipColour {
	kRelCNone,
	kRelCCompliment,
	kRelCAnalogous,
	kRelCTriadic,
	kRelCTetradic,
	kRelCContrast
};

enum RelationshipBright {
	kRelBNone,
	kRelBInvert,
	kRelBDarker,
	kRelBLighter
};

enum RelationshipSat {
	kRelSNone,
	kRelSInvert,
	kRelSLower,
	kRelSHigher
};

/* Component
 * A component represents a single colour in the palette
 */
struct Component {
	wxString _name = "";
	wxColour _clr = wxColour(0,0,0,0);
	int _group = -1;
	bool _setByUser = false;

	int _reference = -1;
	RelationshipColour _relationC = kRelCNone;
	RelationshipBright _relationB = kRelBNone;
	RelationshipSat _relationS = kRelSNone;

	bool _restrictBright = false;
	std::tuple<float, float> _rangeBright;

	bool _restrictSat = false;
	std::tuple<float, float> _rangeSat;

	Background _background = kBackNone;

	Component(wxString name) {
		_name = name;
	}

	Component(wxString name, int group, Background background) {
		_name = name;
		_group = group;
		_background = background;
	}

	Component(wxString name, int group, bool restrictB, std::tuple<float, float> rangeB, bool restrictS, std::tuple<float, float> rangeS, Background background) {
		_name = name;
		_group = group;
		_restrictBright = restrictB;
		_rangeBright = rangeB;
		_restrictSat = restrictS;
		_rangeSat = rangeS;		
		_background = background;
	}

	Component(wxString name, int group, int ref, RelationshipColour relC, RelationshipBright relB, RelationshipSat relS, Background background) {
		_name = name;
		_group = group;
		_reference = ref;
		_relationC = relC;
		_relationB = relB;
		_relationS = relS;
		_background = background;
	}
};

/* Templates
 * A template represents the palette (components) + the preview image (including it's default palette)
 */
struct Template {
	std::vector<Component> _components;
	wxByte *_imageBytes;
	int _imageSize = 0;
	std::vector<wxColour> _imagePalette;
};


/* Palette Renderer
 * This class overwrites the string renderer used in grids
 * This lets us display a colour in the square instead of a string
 */
class PaletteRenderer : public wxGridCellStringRenderer {
public:
    virtual void Draw(wxGrid& grid, wxGridCellAttr& attr, wxDC& dc, const wxRect& rect, int row, int col, bool isSelected) wxOVERRIDE;
};

/* Palette Table
 * This class overwrites the string table base used in grids
 * Instead of just showing strings, we need to show colour as well
 */
class PaletteTable : public wxGridTableBase {
public:
	Template *_template;

	PaletteTable(Template *temp) {
		_template = temp;
	}

	int GetNumberRows() wxOVERRIDE {
		return _template->_components.size();
	}

	int GetNumberCols() wxOVERRIDE {
		return 4;
	}

	wxString GetValue(int row, int col) wxOVERRIDE;
	void SetValue(int row, int col, const wxString &value) wxOVERRIDE;

	bool IsEmptyCell(int row, int col) wxOVERRIDE { return false; }
	wxColour getColour(int row, int col);
	void setColour(int row, int col, wxColour clr);

	void generatePalette();
	void resolvePalette(std::vector<std::vector<Component *>> palette);
	void resolveBackgrounds(std::vector<std::vector<Component *>> palette);
	void resolveConstraints();
	void generateTheme(int theme);
	void generateColourType(int colourType);
};


#endif