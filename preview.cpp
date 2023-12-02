#include "preview.h"

ImagePanel::ImagePanel(wxPanel *parent, wxByte *imageString, int size) : wxPanel(parent) {
	wxBitmap bitmap = wxBitmap::NewFromPNGData(imageString, size);

	// And finally, we convert that to a wxImage which we can resize and use later
	_image = bitmap.ConvertToImage();

	// First we want to grab the width and height of the image
	_imageW = _image.GetWidth();
	_imageH = _image.GetHeight();

	// And for scaling we want the aspect ratio as well
	_aspectRatio = (double) (_imageW) / (double) _imageH;

	_width = -1;
	_height = -1;

	_paletteImage = _image.Copy();

	Bind(wxEVT_PAINT, &ImagePanel::paintEvent, this);
	Bind(wxEVT_SIZE, &ImagePanel::onSize, this);
}

void ImagePanel::updatePalette(std::vector<wxColour> palette, std::vector<Component> components) {
	for (int i = 0; i < palette.size(); i++) {
		if (components[i]._clr == wxNullColour) {
			components[i]._clr = wxColour(0,0,0);
		}
	}
	for (int y = 0; y < _imageH; y++) {
		for (int x = 0; x < _imageW; x++) {
			// We start by setting the potential new colour to 0, and getting the initial distances and similarity values
			int newColour = 0;
			double currentDist = std::sqrt(std::pow(palette[0].Red() - _image.GetRed(x,y), 2) + std::pow(palette[0].Green() - _image.GetGreen(x,y), 2) + std::pow(palette[0].Blue() - _image.GetBlue(x,y), 2));
			double maxDist = std::sqrt(std::pow(255, 2) + std::pow(255, 2) + std::pow(255, 2));
			int similarity = (1 - (currentDist / maxDist)) * 100;

			// And then we look at each colour after it, and if the similarity percentage is lower, we make that the new potential colour
			for (int i = 1; i < palette.size(); i++) {
				currentDist = std::sqrt(std::pow(palette[i].Red() - _image.GetRed(x,y), 2) + std::pow(palette[i].Green() - _image.GetGreen(x,y), 2) + std::pow(palette[i].Blue() - _image.GetBlue(x,y), 2));
				int newSimilarity = (1 - (currentDist / maxDist)) * 100;
				if (newSimilarity >= similarity) {
					newColour = i;
					similarity = newSimilarity;
				}
			}

			_paletteImage.SetRGB(x, y, components[newColour]._clr.Red(), components[newColour]._clr.Green(), components[newColour]._clr.Blue());
		}
	}
	Refresh();
}

void ImagePanel::paintEvent(wxPaintEvent &event) {
	wxPaintDC dc(this);
	render(dc);
}

void ImagePanel::render(wxDC &dc) {
	// The size of the dc changes when you resize the window, so the size of the image needs to adjust
	int newWidth = 0;
	int newHeight = 0;
	dc.GetSize(&newWidth, &newHeight);

	int scaledW = newWidth;
	int scaledH = (int) (scaledW / _aspectRatio);

	if (scaledH > newHeight) {
		scaledH = newHeight;
		scaledW = (int) (scaledH * _aspectRatio);
	}

	_resizedImage = wxBitmap(_paletteImage.Scale(scaledW, scaledH));
	_width = scaledW;
	_height = scaledH;

	// Draw the image with the current zoom
    dc.DrawBitmap(_resizedImage, 0, 0, false);
}

void ImagePanel::onSize(wxSizeEvent &event) {
	Refresh();
	event.Skip();
}

