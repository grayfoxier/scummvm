/* ScummVM - Scumm Interpreter
 * Copyright (C) 2002-2003 The ScummVM project
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * $Header$
 *
 */

#include "stdafx.h"
#include "common/util.h"
#include "common/engine.h"
#include "common/file.h"
#include "scumm/scumm.h"

#include "smush_font.h"

SmushFont::SmushFont(bool use_original_colors, bool new_colors) :
	_nbChars(0),
	_color(-1),
	_new_colors(new_colors),
	_original(use_original_colors) {
	for(int i = 0; i < 256; i++)
		_chars[i].src = NULL;
}

SmushFont::~SmushFont() {
	for(int i = 0; i < _nbChars; i++) {
		if(_chars[i].src)
			delete []_chars[i].src;
	}
}

bool SmushFont::loadFont(const char *filename, const char *directory) {
	debug(8, "SmushFont::loadFont() called");

	File file;
	file.open(filename, directory);
	if (file.isOpen() == false) {
		warning("SmushFont::loadFont() Can't open font file: %s/%s", directory, filename);
		return false;
	}

	uint32 tag = file.readUint32BE();
	if (tag != 'ANIM') {
		debug(8, "SmushFont::loadFont() there is no ANIM chunk in font header");
		return false;
	}

	uint32 length = file.readUint32BE();
	byte *dataSrc = (byte *)malloc(length);
	file.read(dataSrc, length);
	file.close();

	if (READ_BE_UINT32(dataSrc) != 'AHDR') {
		debug(8, "SmushFont::loadFont() there is no AHDR chunk in font header");
		free(dataSrc);
		return false;
	}
	
	_nbChars = READ_LE_UINT16(dataSrc + 10);
	uint32 offset = READ_BE_UINT32(dataSrc + 4) + 8;
	for (int l = 0; l < _nbChars; l++) {
		if (READ_BE_UINT32(dataSrc + offset) == 'FRME') {
			offset += 8;
			if (READ_BE_UINT32(dataSrc + offset) == 'FOBJ') {
				_chars[l].width = READ_LE_UINT16(dataSrc + offset + 14);
				_chars[l].height = READ_LE_UINT16(dataSrc + offset + 16);
				_chars[l].src = new byte[_chars[l].width * _chars[l].height + 1000];
				decodeCodec44(_chars[l].src, dataSrc + offset + 22, READ_BE_UINT32(dataSrc + offset + 4) - 14);
				offset += READ_BE_UINT32(dataSrc + offset + 4) + 8;
			} else {
				debug(8, "SmushFont::loadFont(%s, %s) there is no FOBJ chunk in FRME chunk %d (offset %x)", filename, directory, l, offset);
				break;
			}
		} else {
			debug(8, "SmushFont::loadFont(%s, %s) there is no FRME chunk %d (offset %x)", filename, directory, l, offset);
			break;
		}
	}

	free(dataSrc);
	return true;
}

int SmushFont::getCharWidth(byte c) {
	if(c >= 0x80 && g_scumm->_CJKMode) {
		if(g_scumm->_gameId == GID_CMI)
			return 8;
		if(g_scumm->_gameId == GID_DIG)
			return 6;
		return 0;
	}

	if(c >= _nbChars)
		error("invalid character in SmushFont::charWidth : %d (%d)", c, _nbChars);

	return _chars[c].width;
}

int SmushFont::getCharHeight(byte c) {
	if(c >= 0x80 && g_scumm->_CJKMode) {
		if(g_scumm->_gameId == GID_CMI)
			return 16;
		if(g_scumm->_gameId == GID_DIG)
			return 10;
		return 0;
	}

	if(c >= _nbChars)
		error("invalid character in SmushFont::charHeight : %d (%d)", c, _nbChars);

	return _chars[c].height;
}

int SmushFont::getStringWidth(const char *str) {
	int width = 0;

	while(*str) {
		width += getCharWidth(*str++);
	}

	return width;
}

int SmushFont::getStringHeight(const char *str) {
	int ret = 0;

	for(int i = 0; str[i] != 0; i++) {
		int h = getCharHeight(str[i]);
		ret = MAX(ret, h);
	}

	return ret;
}

void SmushFont::decodeCodec44(byte *dst, const byte *src, int length) {
	byte val;
	uint16 size_line, num;

	do {
		size_line = READ_LE_UINT16(src);
		src += 2;
		length -= 2;

		while (size_line != 0) {
			num = *src++;
			val = *src++;
			memset(dst, val, num);
			dst += num;
			length -= 2;
			size_line -= 2;
			if (size_line != 0) {
				num = READ_LE_UINT16(src) + 1;
				src += 2;
				memcpy(dst, src, num);
				dst += num;
				src += num;
				length -= num + 2;
				size_line -= num + 2;
			}
		}
		dst--;

	} while (length > 1);
}

int SmushFont::drawChar(byte *buffer, int dst_width, int x, int y, byte chr) {
	int w = _chars[chr].width;
	int h = _chars[chr].height;
	const byte *src = _chars[chr].src;
	byte *dst = buffer + dst_width * y + x;

	assert(dst_width == g_scumm->_screenWidth);

	if(_original) {
		for(int j = 0; j < h; j++) {
			for(int i = 0; i < w; i++) {
				char value = *src++;
				if(value) dst[i] = value;
			}
			dst += dst_width;
		}
	} else {
		char color = (_color != -1) ? _color : 1;
		if (_new_colors) {
			for(int j = 0; j < h; j++) {
				for(int i = 0; i < w; i++) {
					char value = *src++;
					if(value == -color) {
						dst[i] = 0xFF;
					} else if(value == -31) {
						dst[i] = 0;
					} else if(value) {
						dst[i] = value;
					}
				}
				dst += dst_width;
			}
		} else {
			for(int j = 0; j < h; j++) {
				for(int i = 0; i < w; i++) {
					char value = *src++;
					if(value == 1) {
						dst[i] = color;
					} else if(value) {
						dst[i] = 0;
					}
				}
				dst += dst_width;
			}
		}
	}
	return w;
}

int SmushFont::draw2byte(byte *buffer, int dst_width, int x, int y, int idx) {
	int w = g_scumm->_2byteWidth;
	int h = g_scumm->_2byteHeight;

	byte *src = g_scumm->get2byteCharPtr(idx);
	byte *dst = buffer + dst_width * (y + (g_scumm->_gameId == GID_CMI ? 7 : 2)) + x;
	byte bits = 0;

	if(_original) {
		for(int j = 0; j < h; j++) {
			for(int i = 0; i < w; i++) {
				char value = 1;//*src++;
				if(value) dst[i] = value;
			}
			dst += dst_width;
		}
	} else {
		char color = (_color != -1) ? _color : 1;
		if (_new_colors)
			color = (char)0xff; //FIXME;
		for(int j = 0; j < h; j++) {
			for(int i = 0; i < w; i++) {
				if((i % 8) == 0)
					bits = *src++;
				if (bits & revBitMask[i % 8]) {
					dst[i + 1] = 0;
					dst[i] = color;
				}
			}
			dst += dst_width;
		}
	}
	return w + 1;
}

static char **split(const char *str, char sep) {
	char **ret = new char *[62];
	int n = 0;
	const char *i = str;
	char *j = strchr(i, sep);

	while(j != NULL) {
		assert(n < 60);
		ret[n] = new char[j - i + 1];
		memcpy(ret[n], i, j - i);
		ret[n++][j - i] = 0;
		i = j + 1;
		j = strchr(i, sep);
	}

	int len = strlen(i);
	ret[n] = new char[len + 1];
	memcpy(ret[n], i, len);
	ret[n++][len] = 0;
	ret[n] = 0;

	return ret;
}

void SmushFont::drawSubstring(const char *str, byte *buffer, int dst_width, int x, int y) {
	for(int i = 0; str[i] != 0; i++) {
		if((byte)str[i] >= 0x80 && g_scumm->_CJKMode) {
			x += draw2byte(buffer, dst_width, x, y, (byte)str[i] + 256 * (byte)str[i+1]);
			i++;
		} else
			x += drawChar(buffer, dst_width, x, y, str[i]);
	}
}

void SmushFont::drawStringAbsolute(const char *str, byte *buffer, int dst_width, int x, int y) {
	debug(9, "SmushFont::drawStringAbsolute(%s, %d, %d)", str, x, y);

	while(str) {
		char line[256];
		char *pos = strchr(str, '\n');
		if(pos) {
			memcpy(line, str, pos - str - 1);
			line[pos - str - 1] = 0;
			str = pos + 1;
		} else {
			strcpy(line, str);
			str = 0;
		}
		drawSubstring(line, buffer, dst_width, x, y);
		y += getStringHeight(line);
	}
}

void SmushFont::drawStringCentered(const char *str, byte *buffer, int dst_width, int dst_height, int y, int xmin, int width, int offset) {
	debug(9, "SmushFont::drawStringCentered(%s, %d, %d)", str, xmin, y);

	if ((strchr(str, '\n') != 0)) {
		char *z = strchr(str, '\n');
		*z = 0;
	}
	char **words = split(str, ' ');
	int nb_sub = 0;

	while(words[nb_sub])
		nb_sub++;

	int *sizes = new int[nb_sub];
	int i = 0, max_width = 0, height = 0, nb_subs = 0;

	for(i = 0; i < nb_sub; i++)
		sizes[i] = getStringWidth(words[i]);

	char **substrings = new char *[nb_sub];
	int *substr_widths = new int[nb_sub];
	int space_width = getCharWidth(' ');

	i = 0;
	while(i < nb_sub) {
		int substr_width = sizes[i];
		char *substr = new char[1000];
		strcpy(substr, words[i]);
		int j = i + 1;

		while(j < nb_sub && (substr_width + space_width + sizes[j]) < width) {
			substr_width += sizes[j++] + space_width;
		}

		for(int k = i + 1; k < j; k++) {
			strcat(substr, " ");
			strcat(substr, words[k]);
		}

		substrings[nb_subs] = substr;
		substr_widths[nb_subs++] = substr_width;
		if(substr_width > max_width)
			max_width = substr_width;
		i = j;
		height += getStringHeight(substr);
	}

	delete []sizes;
	for(i = 0; i < nb_sub; i++) {
		delete []words[i];
	}
	delete []words;
	
	max_width = (max_width + 1) >> 1;
	int x = xmin + width / 2;
	x += offset - dst_width / 2;

	if(x < max_width) x = max_width;
	if(x + max_width > dst_width) {
		x = dst_width - max_width;
	}

	if(y + height > dst_height) {
		y = dst_height - height;
	}

	for(i = 0; i < nb_subs; i++) {
		int substr_width = substr_widths[i];
		drawSubstring(substrings[i], buffer, dst_width, x - substr_width / 2, y);
		y += getStringHeight(substrings[i]);
		delete []substrings[i];
	}

	delete []substr_widths;
	delete []substrings;
}

void SmushFont::drawStringWrap(const char *str, byte *buffer, int dst_width, int dst_height, int x, int y, int width) {
	debug(9, "SmushFont::drawStringWrap(%s, %d, %d)", str, x, y);

	if ((strchr(str, '\n') != 0)) {
		char *z = strchr(str, '\n');
		*z = 0;
	}
	char ** words = split(str, ' ');
	int nb_sub = 0;

	while(words[nb_sub])
		nb_sub++;

	int *sizes = new int[nb_sub];
	int i = 0, max_width = 0, height = 0, nb_subs = 0, left_x;

	for(i = 0; i < nb_sub; i++)
		sizes[i] = getStringWidth(words[i]);

	char **substrings = new char *[nb_sub];
	int *substr_widths = new int[nb_sub];
	int space_width = getCharWidth(' ');

	i = 0;
	while(i < nb_sub) {
		int substr_width = sizes[i];
		char *substr = new char[1000];
		strcpy(substr, words[i]);
		int j = i + 1;

		while(j < nb_sub && (substr_width + space_width + sizes[j]) < width) {
			substr_width += sizes[j++] + space_width;
		}

		for(int k = i + 1; k < j; k++) {
			strcat(substr, " ");
			strcat(substr, words[k]);
		}

		substrings[nb_subs] = substr;
		substr_widths[nb_subs++] = substr_width;
		i = j;
		height += getStringHeight(substr);
	}

	delete []sizes;
	for(i = 0; i < nb_sub; i++) {
		delete []words[i];
	}
	delete []words;

	if(y + height > dst_height) {
		y = dst_height - height;
	}

	for(i = 0; i < nb_subs; i++)
		max_width = MAX(max_width, substr_widths[i]);

	if(max_width + x > dst_width)
		left_x = dst_width - max_width + getCharWidth(' ');
	else
		left_x = x;

	if(max_width + left_x > dst_height)
		left_x = dst_width - max_width;

	for(i = 0; i < nb_subs; i++) {
		drawSubstring(substrings[i], buffer, dst_width, left_x, y);
		y += getStringHeight(substrings[i]);
		delete []substrings[i];
	}

	delete []substr_widths;
	delete []substrings;
}

void SmushFont::drawStringWrapCentered(const char *str, byte *buffer, int dst_width, int dst_height, int x, int y, int width) {
	debug(9, "SmushFont::drawStringWrapCentered(%s, %d, %d)", str, x, y);
	
	int max_substr_width = 0;
	if ((strchr(str, '\n') != 0)) {
		char *z = strchr(str, '\n');
		*z = 0;
	}
	char **words = split(str, ' ');
	int nb_sub = 0;

	while(words[nb_sub])
		nb_sub++;

	int *sizes = new int[nb_sub];
	int i = 0, height = 0, nb_subs = 0;

	for(i = 0; i < nb_sub; i++)
		sizes[i] = getStringWidth(words[i]);

	char **substrings = new char *[nb_sub];
	int *substr_widths = new int[nb_sub];
	int space_width = getCharWidth(' ');

	i = 0;
	width = MIN(width, dst_width);
	while(i < nb_sub) {
		int substr_width = sizes[i];
		char *substr = new char[1000];
		strcpy(substr, words[i]);
		int j = i + 1;

		while(j < nb_sub && (substr_width + space_width + sizes[j]) < width) {
			substr_width += sizes[j++] + space_width;
		}

		for(int k = i + 1; k < j; k++) {
			strcat(substr, " ");
			strcat(substr, words[k]);
		}

		substrings[nb_subs] = substr;
		substr_widths[nb_subs++] = substr_width;
		max_substr_width = MAX(substr_width, max_substr_width);
		i = j;
		height += getStringHeight(substr);
	}

	delete []sizes;
	for(i = 0; i < nb_sub; i++) {
		delete []words[i];
	}
	delete []words;

	if(y + height > dst_height) {
		y = dst_height - height;
	}

	x = (dst_width - max_substr_width) / 2;

	for(i = 0; i < nb_subs; i++) {
		int substr_width = substr_widths[i];
		drawSubstring(substrings[i], buffer, dst_width, x + (max_substr_width - substr_width) / 2, y);
		y += getStringHeight(substrings[i]);
		delete []substrings[i];
	}

	delete []substr_widths;
	delete []substrings;
}
