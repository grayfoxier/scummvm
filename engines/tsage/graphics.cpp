/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * $URL$
 * $Id$
 *
 */

#include "tsage/events.h"
#include "tsage/graphics.h"
#include "tsage/resources.h"
#include "tsage/tsage.h"
#include "tsage/core.h"
#include "common/algorithm.h"
#include "graphics/palette.h"
#include "graphics/surface.h"
#include "tsage/globals.h"

namespace tSage {

/**
 * Creates a new graphics surface with the specified area of another surface
 *
 * @src Source surface
 * @bounds Area to backup
 */
GfxSurface *Surface_getArea(GfxSurface &src, const Rect &bounds) {
	assert(bounds.isValidRect());
	GfxSurface *dest = new GfxSurface();
	dest->create(bounds.width(), bounds.height());

	Graphics::Surface srcSurface = src.lockSurface();
	Graphics::Surface destSurface = dest->lockSurface();

	byte *srcP = (byte *)srcSurface.getBasePtr(bounds.left, bounds.top);
	byte *destP = (byte *)destSurface.getBasePtr(0, 0);

	for (int y = bounds.top; y < bounds.bottom; ++y, srcP += srcSurface.pitch, destP += destSurface.pitch)
		Common::copy(srcP, srcP + destSurface.pitch, destP);

	src.unlockSurface();
	dest->unlockSurface();
	return dest;
}

/**
 * Translates a raw image resource into a graphics surface. The caller is then responsible
 * for managing and destroying the surface when done with it
 *
 * @imgData Raw image resource
 * @size Size of the resource
 */
GfxSurface surfaceFromRes(const byte *imgData) {
	Rect r(0, 0, READ_LE_UINT16(imgData), READ_LE_UINT16(imgData + 2));
	GfxSurface s;
	s.create(r.width(), r.height());
	s._centroid.x = READ_LE_UINT16(imgData + 4);
	s._centroid.y = READ_LE_UINT16(imgData + 6);
	s._transColor = *(imgData + 8);

	bool rleEncoded = (imgData[9] & 2) != 0;

	const byte *srcP = imgData + 10;
	Graphics::Surface destSurface = s.lockSurface();
	byte *destP = (byte *)destSurface.getBasePtr(0, 0);

	if (!rleEncoded) {
		Common::copy(srcP, srcP + (r.width() * r.height()), destP);
	} else {
		Common::set_to(destP, destP + (r.width() * r.height()), s._transColor);

		for (int yp = 0; yp < r.height(); ++yp) {
			int width = r.width();
			destP = (byte *)destSurface.getBasePtr(0, yp);

			while (width > 0) {
				uint8 controlVal = *srcP++;
				if ((controlVal & 0x80) == 0) {
					// Copy specified number of bytes

					Common::copy(srcP, srcP + controlVal, destP);
					width -= controlVal;
					srcP += controlVal;
					destP += controlVal;
				} else if ((controlVal & 0x40) == 0) {
					// Skip a specified number of output pixels
					destP += controlVal & 0x3f;
					width -= controlVal & 0x3f;
				} else {
					// Copy a specified pixel a given number of times
					controlVal &= 0x3f;
					int pixel = *srcP++;

					Common::set_to(destP, destP + controlVal, pixel);
					destP += controlVal;
					width -= controlVal;
				}
			}
			assert(width == 0);
		}
	}

	s.unlockSurface();
	return s;
}

GfxSurface surfaceFromRes(int resNum, int rlbNum, int subNum) {
	uint size;
	byte *imgData = _resourceManager->getSubResource(resNum, rlbNum, subNum, &size);
	GfxSurface surface = surfaceFromRes(imgData);
	DEALLOCATE(imgData);

	return surface;
}
/*--------------------------------------------------------------------------*/

void Rect::set(int16 x1, int16 y1, int16 x2, int16 y2) {
	left = x1; top = y1;
	right = x2; bottom = y2;
}

/**
 * Collapses the rectangle in all four directions by the given x and y amounts
 *
 * @dx x amount to collapse x edges by
 * @dy y amount to collapse y edges by
 */
void Rect::collapse(int dx, int dy) {
	left += dx; right -= dx;
	top += dy; bottom -= dy;
}

/**
 * Centers the rectangle at a given position
 *
 * @xp x position for new center
 * @yp y position for new center
 */
void Rect::center(int xp, int yp) {
	moveTo(xp - (width() / 2), yp - (height() / 2));
}

/**
 * Centers the rectangle at the center of a second passed rectangle
 *
 * @r Second rectangle whose center to use
 */
void Rect::center(const Rect &r) {
	center(r.left + (r.width() / 2), r.top + (r.height() / 2));
}

/*
 * Repositions the bounds if necessary so it falls entirely within the passed bounds
 *
 * @r The bounds the current rect should be within
 */
void Rect::contain(const Rect &r) {
	if (left < r.left) translate(r.left - left, 0);
	if (right > r.right) translate(r.right - right, 0);
	if (top < r.top) translate(0, r.top - top);
	if (bottom > r.bottom) translate(0, r.bottom - bottom);
}

/**
 * Resizes and positions a given rect based on raw image data and a passed scaling percentage
 *
 * @frame Raw image frame
 * @xp New x position
 * @yp New y position
 * @percent Scaling percentage
 */
void Rect::resize(const GfxSurface &surface, int xp, int yp, int percent) {
	int xe = surface.getBounds().width() * percent / 100;
	int ye = surface.getBounds().height() * percent / 100;
	this->set(0, 0, xe, ye);

	if (!right) ++right;
	if (!bottom) ++bottom;

	this->moveTo(xp, yp);

	int xd = surface._centroid.x * percent / 100;
	int yd = surface._centroid.y * percent / 100;
	this->translate(-xd, -yd);
}

/**
 * Expands the pane region to contain the specified Rect
 */
void Rect::expandPanes() {
	_globals->_paneRegions[0].uniteRect(*this);
	_globals->_paneRegions[1].uniteRect(*this);
}

/**
 * Serialises the given rect
 */
void Rect::synchronise(Serialiser &s) {
	s.syncAsSint16LE(left);
	s.syncAsSint16LE(top);
	s.syncAsSint16LE(right);
	s.syncAsSint16LE(bottom);
}

/*--------------------------------------------------------------------------*/

GfxSurface::GfxSurface() : _bounds(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT) {
	_disableUpdates = false;
	_screenSurface = false;
	_lockSurfaceCtr = 0;
	_customSurface = NULL;
	_screenSurfaceP = NULL;
}

GfxSurface::GfxSurface(const GfxSurface &s) {
	_lockSurfaceCtr = 0;
	_customSurface = NULL;
	this->operator =(s);
}

GfxSurface::~GfxSurface() {
	if (_customSurface) {
		_customSurface->free();
		delete _customSurface;
	}
}

/**
 * Specifies that the surface will encapsulate the ScummVM screen surface
 */
void GfxSurface::setScreenSurface() {
	_screenSurface = true;
	_customSurface = NULL;
	_lockSurfaceCtr = 0;
}

/**
 * Specifies that the surface should maintain it's own internal surface
 */
void GfxSurface::create(int width, int height) {
	assert((width >= 0) && (height >= 0));
	_screenSurface = false;
	_customSurface = new Graphics::Surface();
	_customSurface->create(width, height, 1);
	_bounds = Rect(0, 0, width, height);
}

/**
 * Locks the surface for access, and returns a raw ScummVM surface to manipulate it
 */
Graphics::Surface GfxSurface::lockSurface() {
	++_lockSurfaceCtr;

	Graphics::Surface *src;
	if (_screenSurface) {
		if (_lockSurfaceCtr == 1)
			_screenSurfaceP = g_system->lockScreen();
		src = _screenSurfaceP;
	} else
		src = _customSurface;
	assert(src);

	// Setup the returned surface either as one pointing to the same pixels as the source, or
	// as a subset of the source one based on the currently set bounds
	Graphics::Surface result;
	result.w = _bounds.width();
	result.h = _bounds.height();
	result.pitch = src->pitch;
	result.bytesPerPixel = src->bytesPerPixel;
	result.pixels = src->getBasePtr(_bounds.left, _bounds.top);

	return result;
}

/**
 * Unlocks the surface after having accessed it with the lockSurface method
 */
void GfxSurface::unlockSurface() {
	assert(_lockSurfaceCtr > 0);
	--_lockSurfaceCtr;

	if ((_lockSurfaceCtr == 0) && _screenSurface) {
		g_system->unlockScreen();
	}
}

/**
 * Fills a specified rectangle on the surface with the specified color
 *
 * @bounds Area to fill
 * @color Color to use
 */
void GfxSurface::fillRect(const Rect &bounds, int color) {
	Graphics::Surface surface = lockSurface();
	surface.fillRect(bounds, color);
	unlockSurface();
}

GfxSurface &GfxSurface::operator=(const GfxSurface &s) {
	assert(_lockSurfaceCtr == 0);
	assert(s._lockSurfaceCtr == 0);

	if (_customSurface) {
		_customSurface->free();
		delete _customSurface;
	}

	_customSurface = s._customSurface;
	_screenSurface = s._screenSurface;
	_disableUpdates = s._disableUpdates;
	_bounds = s._bounds;
	_centroid = s._centroid;
	_transColor = s._transColor;

	if (_customSurface) {
		// Surface owns the internal data, so replicate it so new surface owns it's own
		_customSurface = new Graphics::Surface();
		_customSurface->create(s._customSurface->w, s._customSurface->h, 1);
		const byte *srcP = (const byte *)s._customSurface->getBasePtr(0, 0);
		byte *destP = (byte *)_customSurface->getBasePtr(0, 0);

		Common::copy(srcP, srcP + (_bounds.width() * _bounds.height()), destP);
	}

	return *this;
}

/**
 * Displays a message on-screen until either a mouse or keypress
 */
bool GfxSurface::displayText(const Common::String &msg, const Common::Point &pt) {
	// Set up a new graphics manager
	GfxManager gfxManager;
	gfxManager.activate();
	gfxManager._font._colors.background = 0;
	gfxManager._font._colors.foreground = 7;
	gfxManager._font.setFontNumber(2);

	// Get the area for text display
	Rect textRect;
	gfxManager.getStringBounds(msg.c_str(), textRect, 200);
	textRect.center(pt.x, pt.y);

	// Make a backup copy of the area the text will occupy
	Rect saveRect = textRect;
	saveRect.collapse(-20, -8);
	GfxSurface *savedArea = Surface_getArea(gfxManager.getSurface(), saveRect);

	// Display the text
	gfxManager._font.writeLines(msg.c_str(), textRect, ALIGN_LEFT);

	// Write for a  mouse or keypress
	Event event;
	while (!_globals->_events.getEvent(event, EVENT_BUTTON_DOWN | EVENT_KEYPRESS) && !_vm->getEventManager()->shouldQuit())
		;

	// Restore the display area
	gfxManager.copyFrom(*savedArea, saveRect.left, saveRect.top);
	delete savedArea;

	gfxManager.deactivate();
	return (event.eventType == EVENT_KEYPRESS) && (event.kbd.keycode == Common::KEYCODE_RETURN);
}

/**
 * Loads a quarter of a screen from a resource
 */
void GfxSurface::loadScreenSection(Graphics::Surface &dest, int xHalf, int yHalf, int xSection, int ySection) {
	int screenNum = _globals->_sceneManager._scene->_activeScreenNumber;
	Rect updateRect(0, 0, 160, 100);
	updateRect.translate(xHalf * 160, yHalf * 100);
	int xHalfCount = (_globals->_sceneManager._scene->_backgroundBounds.right + 159) / 160;
	int yHalfCount = (_globals->_sceneManager._scene->_backgroundBounds.bottom + 99) / 100;

	if (xSection < xHalfCount && ySection < yHalfCount) {
		int rlbNum = xSection * yHalfCount + ySection;
		byte *data = _resourceManager->getResource(RES_BITMAP, screenNum, rlbNum);

		for (int y = 0; y < updateRect.height(); ++y) {
			byte *pSrc = data + y * 160;
			byte *pDest = (byte *)dest.getBasePtr(updateRect.left, updateRect.top + y);

			for (int x = 0; x < updateRect.width(); ++x, ++pSrc, ++pDest) {
				*pDest = *pSrc;
			}
		}

		DEALLOCATE(data);
	}
}

/**
 * Returns an array indicating which pixels of a source image horizontally or vertically get
 * included in a scaled image
 */
static int *scaleLine(int size, int srcSize) {
	const int PRECISION_FACTOR = 1000;
	int scale = PRECISION_FACTOR * size / srcSize;
	assert(scale >= 0);
	int *v = new int[size];
	Common::set_to(v, &v[size], -1);

	int distCtr = 0;
	int *destP = v;
	for (int distIndex = 0; distIndex < srcSize; ++distIndex) {
		distCtr += scale;
		while (distCtr >= PRECISION_FACTOR) {
			assert(destP < &v[size]);
			*destP++ = distIndex;
			distCtr -= PRECISION_FACTOR;
		}
	}

	return v;
}

/**
 * Scales a passed surface, creating a new surface with the result
 * @param srcImage		Source image to scale
 * @param NewWidth		New width for scaled image
 * @param NewHeight		New height for scaled image
 * @remarks Caller is responsible for freeing the returned surface
 */
static GfxSurface ResizeSurface(GfxSurface &src, int xSize, int ySize, int transIndex) {
	GfxSurface s;
	s.create(xSize, ySize);

	Graphics::Surface srcImage = src.lockSurface();
	Graphics::Surface destImage = s.lockSurface();

	int *horizUsage = scaleLine(xSize, srcImage.w);
	int *vertUsage = scaleLine(ySize, srcImage.h);

	// Loop to create scaled version
	for (int yp = 0; yp < ySize; ++yp) {
		byte *destP = (byte *)destImage.getBasePtr(0, yp);

		if (vertUsage[yp] == -1) {
			Common::set_to(destP, destP + xSize, transIndex);
		} else {
			const byte *srcP = (const byte *)srcImage.getBasePtr(0, vertUsage[yp]);

			for (int xp = 0; xp < xSize; ++xp) {
				if (horizUsage[xp] != -1) {
					const byte *tempSrcP = srcP + horizUsage[xp];
					*destP++ = *tempSrcP++;
				} else {
					// Pixel overrun at the end of the line
					*destP++ = transIndex;
				}
			}
		}
	}

	// Unlock surfaces
	src.unlockSurface();
	s.unlockSurface();

	// Delete arrays and return surface
	delete[] horizUsage;
	delete[] vertUsage;
	return s;
}

/**
 * Copys an area from one GfxSurface to another
 */
void GfxSurface::copyFrom(GfxSurface &src, Rect srcBounds, Rect destBounds, Region *priorityRegion) {
	GfxSurface srcImage;

	if (srcBounds == src.getBounds())
		srcImage = src;
	else {
		// Set the source image to be the subset specified by the source bounds
		Graphics::Surface srcSurface = src.lockSurface();

		srcImage.create(srcBounds.width(), srcBounds.height());
		Graphics::Surface destSurface = srcImage.lockSurface();

		const byte *srcP = (const byte *)srcSurface.getBasePtr(srcBounds.left, srcBounds.top);
		byte *destP = (byte *)destSurface.pixels;
		for (int yp = srcBounds.top; yp < srcBounds.bottom; ++yp, srcP += srcSurface.pitch, destP += destSurface.pitch) {
			Common::copy(srcP, srcP + srcBounds.width(), destP);
		}

		srcImage.unlockSurface();
		src.unlockSurface();
	}

	if ((destBounds.width() != srcBounds.width()) || (destBounds.height() != srcBounds.height()))
		srcImage = ResizeSurface(srcImage, destBounds.width(), destBounds.height(), src._transColor);

	Graphics::Surface srcSurface = srcImage.lockSurface();
	Graphics::Surface destSurface = lockSurface();

	// Adjust bounds to ensure destination will be on-screen
	int srcX = 0, srcY = 0;
	if (destBounds.left < 0) {
		srcX = -destBounds.left;
		destBounds.left = 0;
	}
	if (destBounds.top < 0) {
		srcY = -destBounds.top;
		destBounds.top = 0;
	}
	if (destBounds.right > destSurface.w)
		destBounds.right = destSurface.w;
	if (destBounds.bottom > destSurface.h)
		destBounds.bottom = destSurface.h;

	if (destBounds.isValidRect()) {
		const byte *pSrc = (const byte *)srcSurface.getBasePtr(srcX, srcY);
		byte *pDest = (byte *)destSurface.getBasePtr(destBounds.left, destBounds.top);

		for (int y = 0; y < destBounds.height(); ++y, pSrc += srcSurface.pitch, pDest += destSurface.pitch) {

			if (!priorityRegion && (src._transColor == -1))
				Common::copy(pSrc, pSrc + destBounds.width(), pDest);
			else {
				const byte *tempSrc = pSrc;
				byte *tempDest = pDest;
				int xp = destBounds.left;

				while (tempSrc < (pSrc + destBounds.width())) {
					if (!priorityRegion || !priorityRegion->contains(Common::Point(
							xp + _globals->_sceneManager._scene->_sceneBounds.left,
							destBounds.top + y + _globals->_sceneManager._scene->_sceneBounds.top))) {
						if (*tempSrc != src._transColor)
							*tempDest = *tempSrc;
					}
					++tempSrc;
					++tempDest;
					++xp;
				}
			}
		}
	}

	unlockSurface();
	srcImage.unlockSurface();
}

void GfxSurface::draw(const Common::Point &pt, Rect *rect) {
	Rect tempRect = getBounds();
	tempRect.translate(-_centroid.x, -_centroid.y);
	tempRect.translate(pt.x, pt.y);

	if (rect) {
		// Only copy needed rect out without drawing
		*rect = tempRect;
	} else {
		// Draw image
		_globals->gfxManager().copyFrom(*this, tempRect, NULL);
	}
}

/*--------------------------------------------------------------------------*/

GfxElement::GfxElement() {
	_owner = NULL;
	_keycode = 0;
	_flags = 0;
}

void GfxElement::setDefaults() {
	_flags = 0;
	_fontNumber = _globals->_gfxFontNumber;
	_colors = _globals->_gfxColors;
	_fontColors = _globals->_fontColors;
}

/**
 * Highlights the specified graphics element
 */
void GfxElement::highlight() {
	// Get a lock on the surface
	GfxManager &gfxManager = _globals->gfxManager();
	Graphics::Surface surface = gfxManager.lockSurface();

	// Scan through the contents of the element, switching any occurances of the foreground
	// color with the background color and vice versa
	Rect tempRect(_bounds);
	tempRect.collapse(2, 2);

	for (int yp = tempRect.top; yp < tempRect.bottom; ++yp) {
		byte *lineP = (byte *)surface.getBasePtr(tempRect.left, yp);
		for (int xp = tempRect.left; xp < tempRect.right; ++xp, ++lineP) {
			if (*lineP == _colors.background) *lineP = _colors.foreground;
			else if (*lineP == _colors.foreground) *lineP = _colors.background;
		}
	}

	// Release the surface
	gfxManager.unlockSurface();
}

/**
 * Fills the background of the specified element with a border frame
 */
void GfxElement::drawFrame() {
	// Get a lock on the surface and save the active font
	GfxManager &gfxManager = _globals->gfxManager();
	gfxManager.lockSurface();

	uint8 bgColor, fgColor;
	if (_flags & GFXFLAG_THICK_FRAME) {
		bgColor = 0;
		fgColor = 0;
	} else {
		bgColor = _fontColors.background;
		fgColor = _fontColors.foreground;
	}

	Rect tempRect = _bounds;
	tempRect.collapse(3, 3);
	tempRect.collapse(-1, -1);
	gfxManager.fillRect(tempRect, _colors.background);

	--tempRect.bottom; --tempRect.right;
	gfxManager.fillArea(tempRect.left, tempRect.top, bgColor);
	gfxManager.fillArea(tempRect.left, tempRect.bottom, fgColor);
	gfxManager.fillArea(tempRect.right, tempRect.top, fgColor);
	gfxManager.fillArea(tempRect.right, tempRect.bottom, fgColor);

	tempRect.collapse(-1, -1);
	gfxManager.fillRect2(tempRect.left + 1, tempRect.top, tempRect.width() - 1, 1, bgColor);
	gfxManager.fillRect2(tempRect.left, tempRect.top + 1, 1, tempRect.height() - 1, bgColor);
	gfxManager.fillRect2(tempRect.left + 1, tempRect.bottom, tempRect.width() - 1, 1, fgColor);
	gfxManager.fillRect2(tempRect.right, tempRect.top + 1, 1, tempRect.height() - 1, fgColor);

	gfxManager.fillArea(tempRect.left, tempRect.top, 0);
	gfxManager.fillArea(tempRect.left, tempRect.bottom, 0);
	gfxManager.fillArea(tempRect.right, tempRect.top, 0);
	gfxManager.fillArea(tempRect.right, tempRect.bottom, 0);

	tempRect.collapse(-1, -1);
	gfxManager.fillRect2(tempRect.left + 2, tempRect.top, tempRect.width() - 3, 1, 0);
	gfxManager.fillRect2(tempRect.left, tempRect.top + 2, 1, tempRect.height() - 3, 0);
	gfxManager.fillRect2(tempRect.left + 2, tempRect.bottom, tempRect.width() - 3, 1, 0);
	gfxManager.fillRect2(tempRect.right, tempRect.top + 2, 1, tempRect.height() - 3, 0);

	gfxManager.unlockSurface();
}

/**
 * Handles events when the control has focus
 *
 * @event Event to process
 */
bool GfxElement::focusedEvent(Event &event) {
	bool highlightFlag = false;

	while (!_vm->getEventManager()->shouldQuit()) {
		g_system->delayMillis(10);

		if (_bounds.contains(event.mousePos)) {
			if (!highlightFlag) {
				// First highlight call to show the highlight
				highlightFlag = true;
				highlight();
			}
		} else if (highlightFlag) {
			// Mouse is outside the element, so remove the highlight
			highlightFlag = false;
			highlight();
		}

		if (_globals->_events.getEvent(event, EVENT_BUTTON_UP))
			break;
	}

	if (highlightFlag) {
		// Mouse is outside the element, so remove the highlight
		highlight();
	}

	return highlightFlag;
}

/*--------------------------------------------------------------------------*/

GfxImage::GfxImage() : GfxElement() {
	_resNum = 0;
	_rlbNum = 0;
	_cursorNum = 0;
}

void GfxImage::setDetails(int resNum, int rlbNum, int cursorNum) {
	_resNum = resNum;
	_rlbNum = rlbNum;
	_cursorNum = cursorNum;
	setDefaults();
}

void GfxImage::setDefaults() {
	GfxElement::setDefaults();

	// Decode the image
	uint size;
	byte *imgData = _resourceManager->getSubResource(_resNum, _rlbNum, _cursorNum, &size);
	_surface = surfaceFromRes(imgData);
	DEALLOCATE(imgData);

	// Set up the display bounds
	Rect imgBounds = _surface.getBounds();
	imgBounds.moveTo(_bounds.left, _bounds.top);
	_bounds = imgBounds;
}

void GfxImage::draw() {
	Rect tempRect = _bounds;
	tempRect.translate(_globals->gfxManager()._topLeft.x, _globals->gfxManager()._topLeft.y);

	_globals->gfxManager().copyFrom(_surface, tempRect);
}

/*--------------------------------------------------------------------------*/

GfxMessage::GfxMessage() : GfxElement() {
	_textAlign = ALIGN_LEFT;
	_width = 0;
}

void GfxMessage::set(const Common::String &s, int width, TextAlign textAlign) {
	_message = s;
	_width = width;
	_textAlign = textAlign;

	setDefaults();
}

void GfxMessage::setDefaults() {
	GfxElement::setDefaults();

	GfxFontBackup font;
	GfxManager &gfxManager = _globals->gfxManager();
	Rect tempRect;

	gfxManager._font.setFontNumber(this->_fontNumber);
	gfxManager.getStringBounds(_message.c_str(), tempRect, _width);

	tempRect.collapse(-1, -1);
	tempRect.moveTo(_bounds.left, _bounds.top);
	_bounds = tempRect;
}

void GfxMessage::draw() {
	GfxFontBackup font;
	GfxManager &gfxManager = _globals->gfxManager();

	// Set the font and color
	gfxManager.setFillFlag(false);
	gfxManager._font.setFontNumber(_fontNumber);
	gfxManager._font._colors.foreground = this->_colors.foreground;

	// Display the text
	gfxManager._font.writeLines(_message.c_str(), _bounds, _textAlign);
}

/*--------------------------------------------------------------------------*/

void GfxButton::setDefaults() {
	GfxElement::setDefaults();

	GfxFontBackup font;
	GfxManager &gfxManager = _globals->gfxManager();
	Rect tempRect;

	// Get the string bounds and round up the x end to a multiple of 16
	gfxManager._font.setFontNumber(this->_fontNumber);
	gfxManager._font.getStringBounds(_message.c_str(), tempRect, 240);
	tempRect.right = ((tempRect.right + 15) / 16) * 16;

	// Set the button bounds to a reduced area
	tempRect.collapse(-3, -3);
	tempRect.moveTo(_bounds.left, _bounds.top);
	_bounds = tempRect;
}

void GfxButton::draw() {
	// Get a lock on the surface and save the active font
	GfxFontBackup font;
	GfxManager &gfxManager = _globals->gfxManager();
	gfxManager.lockSurface();

	// Draw a basic frame for the button
	drawFrame();

	// Set the font and color
	gfxManager._font.setFontNumber(_fontNumber);
	gfxManager._font._colors.foreground = this->_colors.foreground;

	// Display the button's text
	Rect tempRect(_bounds);
	tempRect.collapse(3, 3);
	gfxManager._font.writeLines(_message.c_str(), tempRect, ALIGN_CENTER);

	gfxManager.unlockSurface();
}

bool GfxButton::process(Event &event) {
	switch (event.eventType) {
	case EVENT_BUTTON_DOWN:
		if (!event.handled) {
			if (_bounds.contains(event.mousePos)) {
				bool result = focusedEvent(event);
				event.handled = true;
				return result;
			}
		}
		break;

	case EVENT_KEYPRESS:
		if (!event.handled && (event.kbd.keycode == _keycode)) {
			// TODO: Ensure momentary click operation displays
			highlight();
			g_system->delayMillis(20);
			highlight();

			event.handled = true;
			return true;
		}

	default:
		break;
	}

	return false;
}

/*--------------------------------------------------------------------------*/

GfxDialog::GfxDialog() {
	_savedArea = NULL;
	_defaultButton = NULL;
}

GfxDialog::~GfxDialog() {
	remove();
}

void GfxDialog::setDefaults() {
	GfxElement::setDefaults();

	// Initialise the embedded graphics manager
	_gfxManager.setDefaults();

	// Figure out a rect needed for all the added elements
	GfxElementList::iterator i;
	Rect tempRect;
	for (i = _elements.begin(); i != _elements.end(); ++i)
		tempRect.extend((*i)->_bounds);

	// Set the dialog boundaries
	_gfxManager._bounds = tempRect;
	tempRect.collapse(-6, -6);
	_bounds = tempRect;
}

void GfxDialog::remove() {
	if (_savedArea) {
		// Restore the area the dialog covered
		_globals->_gfxManagerInstance.copyFrom(*_savedArea, _bounds.left, _bounds.top);

		delete _savedArea;
		_savedArea = NULL;
	}
}

void GfxDialog::draw() {
	Rect tempRect(_bounds);

	// Make a backup copy of the area the dialog will occupy
	_savedArea = Surface_getArea(_globals->_gfxManagerInstance.getSurface(), _bounds);

	// Set the palette for use in the dialog
	setPalette();

	_gfxManager.activate();

	// Fill in the contents of the entire dialog
	_gfxManager._bounds = Rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	drawFrame();

	// Reset the dialog's graphics manager to only draw within the dialog boundaries
	tempRect.translate(6, 6);
	_gfxManager._bounds = tempRect;

	// Draw each element in the dialog in order
	GfxElementList::iterator i;
	for (i = _elements.begin(); i != _elements.end(); ++i) {
		(*i)->draw();
	}

	// If there's a default button, then draw it
	if (_defaultButton) {
		_defaultButton->_flags |= GFXFLAG_THICK_FRAME;
		_defaultButton->draw();
	}

	_gfxManager.deactivate();
}

void GfxDialog::add(GfxElement *element) {
	_elements.push_back(element);
	element->_owner = this;
}

void GfxDialog::addElements(GfxElement *ge, ...) {
	va_list va;
	va_start(va, ge);
	GfxElement *gfxElement = ge;
	while (gfxElement) {
		add(gfxElement);

		gfxElement = va_arg(va, GfxElement *);
	}

	va_end(va);
}

void GfxDialog::setTopLeft(int xp, int yp) {
	_bounds.moveTo(xp - 6, yp - 6);
}

void GfxDialog::setCenter(int xp, int yp) {
	setTopLeft(xp - (_bounds.width() / 2), yp - (_bounds.height() / 2));
}

GfxButton *GfxDialog::execute(GfxButton *defaultButton) {
	_gfxManager.activate();

	if (defaultButton != _defaultButton) {
		if (_defaultButton) {
			_defaultButton->_flags &= ~GFXFLAG_THICK_FRAME;
			_defaultButton->draw();
		}
		_defaultButton = defaultButton;
	}
	if (_defaultButton) {
		_defaultButton->_flags |= GFXFLAG_THICK_FRAME;
		_defaultButton->draw();
	}

	// Event loop
	GfxButton *selectedButton = NULL;

	while (!_vm->getEventManager()->shouldQuit()) {
		Event event;
		while (_globals->_events.getEvent(event)) {
			// Adjust mouse positions to be relative within the dialog
			event.mousePos.x -= _gfxManager._bounds.left;
			event.mousePos.y -= _gfxManager._bounds.top;

			for (GfxElementList::iterator i = _elements.begin(); i != _elements.end(); ++i) {
				if ((*i)->process(event))
					selectedButton = static_cast<GfxButton *>(*i);
			}
		}

		if (selectedButton)
			break;
		else if (!event.handled) {
			if ((event.eventType == EVENT_KEYPRESS) && (event.kbd.keycode == Common::KEYCODE_ESCAPE)) {
				selectedButton = NULL;
				break;
			} else if ((event.eventType == EVENT_KEYPRESS) && (event.kbd.keycode == Common::KEYCODE_RETURN)) {
				selectedButton = defaultButton;
				break;
			}
		}
	}

	_gfxManager.deactivate();
	if (_defaultButton)
		_defaultButton->_flags &= ~GFXFLAG_THICK_FRAME;

	return selectedButton;
}

void GfxDialog::setPalette() {
	_globals->_scenePalette.loadPalette(0);
	_globals->_scenePalette.setPalette(0, 1);
	_globals->_scenePalette.setPalette(_globals->_scenePalette._colors.foreground, 1);
	_globals->_scenePalette.setPalette(_globals->_fontColors.background, 1);
	_globals->_scenePalette.setPalette(_globals->_fontColors.foreground, 1);
	_globals->_scenePalette.setPalette(255, 1);
}

/*--------------------------------------------------------------------------*/

GfxManager::GfxManager() : _surface(_globals->_screenSurface), _oldManager(NULL) {
	_font.setOwner(this);
	_font._fillFlag = false;
	_bounds = Rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}

GfxManager::GfxManager(GfxSurface &s) : _surface(s), _oldManager(NULL) {
	_font.setOwner(this);
	_font._fillFlag = false;
}

void GfxManager::setDefaults() {
	Rect screenBounds(0, 0, g_system->getWidth(), g_system->getHeight());

	_surface.setBounds(screenBounds);
	_bounds = screenBounds;
	_pane0Rect4 = screenBounds;

	_font._edgeSize = Common::Point(1, 1);
	_font._colors = _globals->_fontColors;
	_font.setFontNumber(_globals->_gfxFontNumber);
}

void GfxManager::activate() {
	assert(!contains(_globals->_gfxManagers, this));
	_globals->_gfxManagers.push_front(this);
}

void GfxManager::deactivate() {
	// Assert that there will still be another manager, and we're correctly removing our own
	assert((_globals->_gfxManagers.size() > 1) && (&_globals->gfxManager() == this));
	_globals->_gfxManagers.pop_front();
}

int GfxManager::getStringWidth(const char *s, int numChars) {
	return _font.getStringWidth(s, numChars);
}

int GfxManager::getStringWidth(const char *s) {
	return _font.getStringWidth(s);
}

void GfxManager::getStringBounds(const char *s, Rect &bounds, int maxWidth) {
	_font.getStringBounds(s, bounds, maxWidth);
}

void GfxManager::fillArea(int xp, int yp, int color) {
	_surface.setBounds(_bounds);
	Rect tempRect(xp, yp, xp + _font._edgeSize.x, yp + _font._edgeSize.y);
	_surface.fillRect(tempRect, color);
}

void GfxManager::fillRect(const Rect &bounds, int color) {
	_surface.setBounds(_bounds);
	_surface.fillRect(bounds, color);
}

void GfxManager::fillRect2(int xs, int ys, int width, int height, int color) {
	_surface.setBounds(_bounds);
	_surface.fillRect(Rect(xs, ys, xs + width, ys + height), color);
}

/**
 * Sets up the standard palette for dialog displays
 */
void GfxManager::setDialogPalette() {
	// Get the main palette information
	byte palData[256 * 3];
	uint count, start;
	_resourceManager->getPalette(0, &palData[0], &start, &count);
	g_system->getPaletteManager()->setPalette(&palData[0], start, count);

	// Miscellaneous
	uint32 white = 0xffffffff;
	g_system->getPaletteManager()->setPalette((const byte *)&white, 255, 1);
}

/**
 * Returns the angle of line connecting two points
 */
int GfxManager::getAngle(const Common::Point &p1, const Common::Point &p2) {
	int xDiff = p2.x - p1.x, yDiff = p1.y - p2.y;

	if (!xDiff && !yDiff)
		return -1;
	else if (!xDiff)
		return (p2.y >= p1.y) ? 180 : 0;
	else if (!yDiff)
		return (p2.x >= p1.x) ? 90 : 270;
	else {
		int result = (((xDiff * 100) / ((abs(xDiff) + abs(yDiff))) * 90) / 100);

		if (yDiff < 0)
			result = 180 - result;
		else if (xDiff < 0)
			result += 360;

		return result;
	}
}
/*--------------------------------------------------------------------------*/


GfxFont::GfxFont() {
	_fontNumber = (_vm->getFeatures() & GF_DEMO) ? 0 : 50;
	_numChars = 0;
	_bpp = 0;
	_fontData = NULL;
	_fillFlag = false;
}

GfxFont::~GfxFont() {
	DEALLOCATE(_fontData);
}

/**
 * Sets the current active font number
 *
 * @fontNumber New font number
 */
void GfxFont::setFontNumber(uint32 fontNumber) {
	if ((_fontNumber == fontNumber) && (_fontData))
		return;

	DEALLOCATE(_fontData);

	_fontNumber = fontNumber;

	_fontData = _resourceManager->getResource(RES_FONT, _fontNumber, 0, true);
	if (!_fontData)
		_fontData = _resourceManager->getResource(RES_FONT, _fontNumber, 0);

	_numChars = READ_LE_UINT16(_fontData + 4);
	_fontSize.y = READ_LE_UINT16(_fontData + 6);
	_fontSize.x = READ_LE_UINT16(_fontData + 8);
	_bpp = READ_LE_UINT16(_fontData + 10);
}

/**
 * Returns the width of the given specified character
 *
 * @ch Character to return width of
 */
int GfxFont::getCharWidth(char ch) {
	assert(_numChars > 0);
	uint32 charOffset = READ_LE_UINT32(_fontData + 12 + (uint8)ch * 4);
	return _fontData[charOffset] & 0x1f;
}

/**
 * Returns the width of the given string in the current font
 *
 * @s String to return the width of
 * @numChars Number of characters within the string to use
 */
int GfxFont::getStringWidth(const char *s, int numChars) {
	assert(_numChars > 0);
	int width = 0;

	for (; numChars > 0; --numChars, ++s) {
		uint32 charOffset = READ_LE_UINT32(_fontData + 12 + (uint8)*s * 4);
		int charWidth = _fontData[charOffset] & 0x1f;

		width += charWidth;
	}

	return width;
}

/**
 * Returns the width of the given string in the current font
 *
 * @s String to return the width of
 */
int GfxFont::getStringWidth(const char *s) {
	return getStringWidth(s, strlen(s));
}

/**
 * Returns the maximum number of characters for words that will fit into a given width
 *
 * @s Message to be analysed
 * @maxWidth Maximum allowed width
 */
int GfxFont::getStringFit(const char *&s, int maxWidth) {
	const char *nextWord = NULL;
	const char *sStart = s;
	int numChars = 1;
	int strWidth = 1;
	char nextChar;

	for (;;) {
		nextChar = *s++;

		if ((nextChar == '\r') || (nextChar == '\0'))
			break;

		// Check if it's a word end
		if (nextChar == ' ') {
			nextWord = s;
		}

		strWidth = getStringWidth(sStart, numChars);
		if (strWidth > maxWidth) {
			if (nextWord) {
				s = nextWord;
				nextChar = ' ';
			}
			break;
		}

		++numChars;
	}

	int totalChars = s - sStart;
	if (nextChar == '\0')
		--s;
	if ((nextChar == ' ') || (nextChar == '\r') || (nextChar == '\0'))
		--totalChars;

	return totalChars;
}

/**
 * Fills out the passed rect with the dimensions of a given string word-wrapped to a
 * maximum specified width
 *
 * @s Message to be analysed
 * @bounds Rectangle to put output size into
 * @maxWidth Maximum allowed line width in pixels
 */
void GfxFont::getStringBounds(const char *s, Rect &bounds, int maxWidth) {
	if (maxWidth == 0) {
		// No maximum width, so set bounds for a single line
		bounds.set(0, 0, getStringWidth(s), getHeight());
	} else {
		int numLines = 0;
		int lineWidth = 0;

		// Loop to figure out the number of lines required, and the maximum line width
		while (*s) {
			const char *msg = s;
			int numChars = getStringFit(msg, maxWidth);
			lineWidth = MAX(lineWidth, getStringWidth(s, numChars));

			s = msg;
			++numLines;
		}

		bounds.set(0, 0, lineWidth, numLines * getHeight());
	}
}

/**
 * Writes out a character at the currently set position using the active font
 *
 * @ch Character to display
 */
int GfxFont::writeChar(const char ch) {
	assert((_fontData != NULL) && ((uint8)ch < _numChars));
	uint32 charOffset = READ_LE_UINT32(_fontData + 12 + (uint8)ch * 4);
	int charWidth = _fontData[charOffset] & 0x1f;
	int charHeight = (READ_LE_UINT16(_fontData + charOffset) >> 5) & 0x3f;
	int yOffset = (_fontData[charOffset + 1] >> 3) & 0x1f;
	const uint8 *dataP = &_fontData[charOffset + 2];

	// Lock the surface for access
	Graphics::Surface surfacePtr = _gfxManager->lockSurface();

	Rect charRect;
	charRect.set(0, 0, charWidth, _fontSize.y);
	charRect.translate(_topLeft.x + _position.x, _topLeft.y + _position.y + yOffset);

	if (_fillFlag)
		surfacePtr.fillRect(charRect, _colors.background);

	charRect.bottom = charRect.top + charHeight;

	// Display the character
	int bitCtr = 0;
	uint8 v = 0;
	for (int yp = charRect.top; yp < charRect.bottom; ++yp) {
		byte *destP = (byte *)surfacePtr.getBasePtr(charRect.left, yp);

		for (int xs = 0; xs < charRect.width(); ++xs, ++destP) {
			// Get the next color index to use
			if ((bitCtr % 8) == 0) v = *dataP++;
			int colIndex = 0;
			for (int subCtr = 0; subCtr < _bpp; ++subCtr, ++bitCtr) {
				colIndex = (colIndex << 1) | (v & 0x80 ? 1 : 0);
				v <<= 1;
			}

			switch (colIndex) {
			//case 0: *destP = _colors.background; break;
			case 1: *destP = _colors.foreground; break;
			case 2: *destP = _colors2.background; break;
			case 3: *destP = _colors2.foreground; break;
			}
		}
	}

	_position.x += charWidth;
	_gfxManager->unlockSurface();
	return charWidth;
}

/**
 * Writes the specified number of characters from the specified string at the current text position
 *
 * @s String to display
 * @numChars Number of characters to print
 */
void GfxFont::writeString(const char *s, int numChars) {
	// Lock the surface for access
	_gfxManager->lockSurface();

	while ((numChars-- > 0) && (*s != '\0')) {
		writeChar(*s);
		++s;
	}

	// Release the surface lock
	_gfxManager->unlockSurface();
}

/**
 * Writes the the specified string at the current text position
 *
 * @s String to display
 */
void GfxFont::writeString(const char *s) {
	writeString(s, strlen(s));
}

/**
 * Writes a specified string within a given area with support for word wrapping and text alignment types
 *
 * @s String to display
 * @bounds Bounds to display the text within
 * @align Text alignment mode
 */
void GfxFont::writeLines(const char *s, const Rect &bounds, TextAlign align) {
	int lineNum = 0;

	// Lock the surface for access
	_gfxManager->lockSurface();

	while (*s) {
		const char *msgP = s;
		int numChars = getStringFit(msgP, bounds.width());

		_position.y = bounds.top + lineNum * getHeight();

		switch (align) {
		case ALIGN_RIGHT:
			// Right aligned text
			_position.x = bounds.right - getStringWidth(s, numChars);
			writeString(s, numChars);
			break;

		case ALIGN_CENTER:
			// Center aligned text
			_position.x = bounds.left + (bounds.width() / 2) - (getStringWidth(s, numChars) / 2);
			writeString(s, numChars);
			break;

		case ALIGN_JUSTIFIED: {
			// Justified text
			// Get the number of words in the string portion
			int charCtr = 0, numWords = 0;
			while (charCtr < numChars) {
				if (s[charCtr] == ' ')
					++numWords;
				++charCtr;
			}
			// If end of string, count final word
			if (*msgP == '\0')
				++numWords;

			// Display the words of the string
			int spareWidth = bounds.width() - getStringWidth(s, numChars);
			charCtr = 0;
			_position.x = bounds.left;

			while (charCtr < numChars) {
				writeChar(s[charCtr]);
				if ((numWords > 0) && (s[charCtr] == ' ')) {
					int separationWidth = spareWidth / numWords;
					spareWidth -= separationWidth;
					--numWords;
					_position.x += separationWidth;
				}

				++charCtr;
			}
			break;
		}

		case ALIGN_LEFT:
		default:
			// Standard text
			_position.x = bounds.left;
			writeString(s, numChars);
			break;
		}

		// Next line
		s = msgP;
		++lineNum;
	}

	// Release the surface lock
	_gfxManager->unlockSurface();
}

/*--------------------------------------------------------------------------*/

GfxFontBackup::GfxFontBackup() {
	_edgeSize = _globals->gfxManager()._font._edgeSize;
	_position = _globals->gfxManager()._font._position;
	_colors = _globals->gfxManager()._font._colors;
	_fontNumber = _globals->gfxManager()._font._fontNumber;
}

GfxFontBackup::~GfxFontBackup() {
	_globals->gfxManager()._font.setFontNumber(_fontNumber);
	_globals->gfxManager()._font._edgeSize = _edgeSize;
	_globals->gfxManager()._font._position = _position;
	_globals->gfxManager()._font._colors = _colors;
}


} // End of namespace tSage
