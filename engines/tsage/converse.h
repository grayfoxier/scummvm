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

#ifndef TSAGE_CONVERSE_H
#define TSAGE_CONVERSE_H

#include "tsage/core.h"
#include "tsage/dialogs.h"

namespace tSage {

class StripCallback : public Action {
public:
	virtual void stripCallback(int v) = 0;
};

class SequenceManager : public Action {
private:
	void setup();
	uint16 getNextValue();
	void setMessage(int resNum, int lineNum, int color, const Common::Point &pt, int width);
	SequenceManager *globalManager();
public:
	SceneText _sceneText;
	int _resNum;
	uint _sequenceOffset;
	bool _keepActive;
	int _field24;
	int _field26;
	Common::Array<byte> _sequenceData;
	int _objectIndex;
	SceneObject *_sceneObject;
	SceneObject *_objectList[6];
	SoundHandler _soundHandler;
public:
	SequenceManager();

	virtual Common::String getClassName() { return "SequenceManager"; }
	virtual void synchronise(Serialiser &s);
	virtual void remove();
	virtual void signal();
	virtual void process(Event &event);
	virtual void attached(EventHandler *newOwner, EventHandler *endHandler, va_list va);
};


class Speaker : public EventHandler {
public:
	Rect _fieldA;
	Action *_field18;
	Common::String _speakerName;
	int _newSceneNumber;
	int _oldSceneNumber;
	SceneObjectList _objectList;
	Rect _sceneBounds;
	SceneText _sceneText;
	int _textWidth;
	Common::Point _textPos;
	int _fontNumber;
	TextAlign _textMode;
	int _color1, _color2, _color3;
	bool _hideObjects;
public:
	Speaker();

	virtual Common::String getClassName() { return "Speaker"; }
	virtual void synchronise(Serialiser &s);
	virtual void remove();
	virtual void proc12(Action *action);
	virtual void setText(const Common::String &msg);
	virtual void removeText();

	void setTextPos(const Common::Point &pt) { _textPos = pt; }
};

class SpeakerGameText : public Speaker {
public:
	SpeakerGameText();

	virtual Common::String getClassName() { return "SpeakerGameText"; }
};

class ScreenSpeaker : public Speaker {
public:
	SceneItem *_npc;
public:
	ScreenSpeaker();

	virtual Common::String getClassName() { return "ScreenSpeaker"; }
	virtual void setText(const Common::String &msg);
};

class SpeakerAction : public Action {
public:
	virtual void signal();

	virtual Common::String getClassName() { return "SpeakerAction"; }
};

class AnimatedSpeaker : public Speaker {
public:
	SceneObject _object1;
	SceneObject _object2;
	SpeakerAction _speakerAction;
public:
	virtual Common::String getClassName() { return "AnimatedSpeaker"; }
	virtual void removeText();
};

class ChoiceEntry {
public:
	Common::String _msg;
	Rect _bounds;

	ChoiceEntry() {}
	ChoiceEntry(const Common::String &msg, const Rect &bounds) {
		_msg = msg;
		_bounds = bounds;
	}
};

class ConversationChoiceDialog : public ModalDialog {
public:
	int _stdColor;
	int _highlightColor;
	int _fontNumber;
	int _savedFgColor;
	int _savedFontNumber;
	Common::Array<ChoiceEntry> _choiceList;
	uint _selectedIndex;
public:
	ConversationChoiceDialog();

	void setColors(int stdColor, int highlightColor) {
		_stdColor = stdColor;
		_highlightColor = highlightColor;
	}
	void setFontNumber(int fontNum) { _fontNumber = fontNum; }
	int execute(const Common::StringArray &choiceList);

	virtual void draw();
};

class Obj0A : public Serialisable {
public:
	int _id;
	uint _scriptOffset;

	virtual void synchronise(Serialiser &s) {
		s.syncAsSint32LE(_id);
		s.syncAsUint32LE(_scriptOffset);
	}
};

#define OBJ44_LIST_SIZE 5

class Obj44 : public Serialisable {
public:
	int _id;
	int _field2[OBJ44_LIST_SIZE];
	Obj0A _list[OBJ44_LIST_SIZE];
	uint _speakerOffset;
public:
	void load(const byte *dataP);
	virtual void synchronise(Serialiser &s);
};

class StripManager : public Action {
private:
	void reset();
	void load();
	Speaker *getSpeaker(const char *speakerName);
	int getNewIndex(int newId);
public:
	int _stripNum;
	int _obj44Index;
	int _field20;
	int _sceneNumber;
	Rect _sceneBounds;
	ConversationChoiceDialog _choiceDialog;
	Common::Array<Speaker *> _speakerList;
	StripCallback *_callbackObject;
	Speaker *_activeSpeaker;
	bool _textShown;
	bool _field2E6;
	int _field2E8;
	Common::Array<Obj44> _obj44List;
	Common::Array<byte> _script;
public:
	StripManager();
	virtual ~StripManager();

	virtual void synchronise(Serialiser &s);
	virtual void remove();
	virtual void signal();
	virtual void process(Event &event);

	void start(int stripNum, EventHandler *owner, StripCallback *callback = NULL);
	void setCallback(StripCallback *callback) { _callbackObject = callback; }
	void setColors(int stdColor, int highlightColor) { _choiceDialog.setColors(stdColor, highlightColor); }
	void setFontNumber(int fontNum) { _choiceDialog.setFontNumber(fontNum); }
	void addSpeaker(Speaker *speaker);
};

} // End of namespace tSage

#endif
