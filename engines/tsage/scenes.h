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

#ifndef TSAGE_SCENES_H
#define TSAGE_SCENES_H

#include "common/scummsys.h"
#include "tsage/converse.h"
#include "tsage/events.h"
#include "tsage/core.h"
#include "tsage/saveload.h"

namespace tSage {

class Scene : public StripCallback {
private:
	void drawAltObjects();
public:
	int _field12;
	int _screenNumber;
	int _activeScreenNumber;
	int _sceneMode;
	StripManager _stripManager;

	Rect _backgroundBounds;
	GfxSurface _backSurface;
	Rect _sceneBounds;
	Rect _oldSceneBounds;
	int _enabledSections[256];
	int _zoomPercents[256];
	ScenePriorities _priorities;

	int _fieldA;
	int _fieldE;
public:
	Scene();
	virtual ~Scene();

	virtual Common::String getClassName() { return "Scene"; }
	virtual void synchronise(Serialiser &s);
	virtual void stripCallback(int v) {}
	virtual void postInit(SceneObjectList *OwnerList = NULL);
	virtual void process(Event &event);
	virtual void dispatch();
	virtual void loadScene(int sceneNum);

	void setZoomPercents(int yStart, int minPercent, int yEnd, int maxPercent);
	void loadBackground(int xAmount, int yAmount);
	void refreshBackground(int xAmount, int yAmount);
	void loadSceneData(int sceneNum);
};

class SceneManager : public GameHandler, public SaveListener {
private:
	void disposeRegions() { warning("TODO: disposeRegions()"); }
	Scene *getNewScene();
public:
	Scene *_scene;
	bool _hasPalette;
	int _loadMode;
	int _sceneNumber;
	int _previousScene;
	int _nextSceneNumber;
	FadeMode _fadeMode;
	Common::Point _sceneBgOffset;
	int _sceneLoadCount;
	Rect _scrollerRect;
	SceneObjectList _altSceneObjects;
	int _objectCount;
public:
	SceneManager();
	virtual ~SceneManager();

	virtual void listenerSynchronise(Serialiser &s);
	void setNewScene(int sceneNumber);
	void checkScene();
	void sceneChange();
	void fadeInIfNecessary();
	void changeScene(int newSceneNumber);
	void setBgOffset(const Common::Point &pt, int loadCount);

	void removeAction(Action *action) {
		// Not currently implemented because addAction method doesn't seem to have any callers
	}

	static void setup();
	static void setBackSurface();
	static void saveListener(int saveMode);
	static void loadNotifier(bool postFlag);
};

} // End of namespace tSage

#endif
