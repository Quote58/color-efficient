// Preview Image

#ifndef IMAGE_H
#define IMAGE_H

// For compilers that don't support precompilation, include "wx/wx.h"
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include "palette.h"

class ImagePanel : public wxPanel {
public:
	wxImage _image;
	wxImage _paletteImage;
	wxBitmap _resizedImage;
	int _imageW = 0;
	int _imageH = 0;
	double _aspectRatio = 0;

	int _width = 0;
	int _height = 0;

	ImagePanel(wxPanel *parent, wxByte *imageString, int size);

	void paintEvent(wxPaintEvent &event);
	void paintNow();
	void onSize(wxSizeEvent &event);
	void render(wxDC &dc);
	void updatePalette(std::vector<wxColour> palette, std::vector<Component> components);
};

#endif