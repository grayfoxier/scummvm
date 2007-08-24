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

// Scripting module script function component

#include "saga/saga.h"

#include "saga/gfx.h"
#include "saga/actor.h"
#include "saga/animation.h"
#include "saga/console.h"
#include "saga/events.h"
#include "saga/font.h"
#include "saga/interface.h"
#include "saga/music.h"
#include "saga/itedata.h"
#include "saga/puzzle.h"
#include "saga/render.h"
#include "saga/sound.h"
#include "saga/sndres.h"
#include "saga/rscfile.h"

#include "saga/script.h"
#include "saga/objectmap.h"

#include "saga/scene.h"
#include "saga/isomap.h"
#include "saga/sagaresnames.h"

#include "common/config-manager.h"

namespace Saga {

#define OPCODE(x) {&Script::x, #x}

void Script::setupScriptFuncList(void) {
	static const ScriptFunctionDescription ITEscriptFunctionsList[ITE_SCRIPT_FUNCTION_MAX] = {
		OPCODE(sfPutString),
		OPCODE(sfWait),
		OPCODE(sfTakeObject),
		OPCODE(sfIsCarried),
		OPCODE(sfStatusBar),
		OPCODE(sfMainMode),
		OPCODE(sfScriptWalkTo),
		OPCODE(sfScriptDoAction),
		OPCODE(sfSetActorFacing),
		OPCODE(sfStartBgdAnim),
		OPCODE(sfStopBgdAnim),
		OPCODE(sfLockUser),
		OPCODE(sfPreDialog),
		OPCODE(sfKillActorThreads),
		OPCODE(sfFaceTowards),
		OPCODE(sfSetFollower),
		OPCODE(sfScriptGotoScene),
		OPCODE(sfSetObjImage),
		OPCODE(sfSetObjName),
		OPCODE(sfGetObjImage),
		OPCODE(sfGetNumber),
		OPCODE(sfScriptOpenDoor),
		OPCODE(sfScriptCloseDoor),
		OPCODE(sfSetBgdAnimSpeed),
		OPCODE(SF_cycleColors),
		OPCODE(sfDoCenterActor),
		OPCODE(sfStartBgdAnimSpeed),
		OPCODE(sfScriptWalkToAsync),
		OPCODE(sfEnableZone),
		OPCODE(sfSetActorState),
		OPCODE(sfScriptMoveTo),
		OPCODE(sfSceneEq),
		OPCODE(sfDropObject),
		OPCODE(sfFinishBgdAnim),
		OPCODE(sfSwapActors),
		OPCODE(sfSimulSpeech),
		OPCODE(sfScriptWalk),
		OPCODE(sfCycleFrames),
		OPCODE(sfSetFrame),
		OPCODE(sfSetPortrait),
		OPCODE(sfSetProtagPortrait),
		OPCODE(sfChainBgdAnim),
		OPCODE(sfScriptSpecialWalk),
		OPCODE(sfPlaceActor),
		OPCODE(sfCheckUserInterrupt),
		OPCODE(sfScriptWalkRelative),
		OPCODE(sfScriptMoveRelative),
		OPCODE(sfSimulSpeech2),
		OPCODE(sfPlacard),
		OPCODE(sfPlacardOff),
		OPCODE(sfSetProtagState),
		OPCODE(sfResumeBgdAnim),
		OPCODE(sfThrowActor),
		OPCODE(sfWaitWalk),
		OPCODE(sfScriptSceneID),
		OPCODE(sfChangeActorScene),
		OPCODE(sfScriptClimb),
		OPCODE(sfSetDoorState),
		OPCODE(sfSetActorZ),
		OPCODE(sfScriptText),
		OPCODE(sfGetActorX),
		OPCODE(sfGetActorY),
		OPCODE(sfEraseDelta),
		OPCODE(sfPlayMusic),
		OPCODE(sfPickClimbOutPos),
		OPCODE(sfTossRif),
		OPCODE(sfShowControls),
		OPCODE(sfShowMap),
		OPCODE(sfPuzzleWon),
		OPCODE(sfEnableEscape),
		OPCODE(sfPlaySound),
		OPCODE(sfPlayLoopedSound),
		OPCODE(sfGetDeltaFrame),
		OPCODE(sfShowProtect),
		OPCODE(sfProtectResult),
		OPCODE(sfRand),
		OPCODE(sfFadeMusic),
		OPCODE(sfPlayVoice)
	};

static const ScriptFunctionDescription IHNMscriptFunctionsList[IHNM_SCRIPT_FUNCTION_MAX] = {
		OPCODE(sfNull),
		OPCODE(sfWait),
		OPCODE(sfTakeObject),
		OPCODE(sfIsCarried),
		OPCODE(sfStatusBar),
		OPCODE(sfMainMode),
		OPCODE(sfScriptWalkTo),
		OPCODE(sfScriptDoAction),
		OPCODE(sfSetActorFacing),
		OPCODE(sfStartBgdAnim),
		OPCODE(sfStopBgdAnim),
		OPCODE(sfLockUser),
		OPCODE(sfPreDialog),
		OPCODE(sfKillActorThreads),
		OPCODE(sfFaceTowards),
		OPCODE(sfSetFollower),
		OPCODE(sfScriptGotoScene),
		OPCODE(sfSetObjImage),
		OPCODE(sfSetObjName),
		OPCODE(sfGetObjImage),
		OPCODE(sfGetNumber),
		OPCODE(sfScriptOpenDoor),
		OPCODE(sfScriptCloseDoor),
		OPCODE(sfSetBgdAnimSpeed),
		OPCODE(SF_cycleColors),
		OPCODE(sfDoCenterActor),
		OPCODE(sfStartBgdAnimSpeed),
		OPCODE(sfScriptWalkToAsync),
		OPCODE(sfEnableZone),
		OPCODE(sfSetActorState),
		OPCODE(sfScriptMoveTo),
		OPCODE(sfSceneEq),
		OPCODE(sfDropObject),
		OPCODE(sfFinishBgdAnim),
		OPCODE(sfSwapActors),
		OPCODE(sfSimulSpeech),
		OPCODE(sfScriptWalk),
		OPCODE(sfCycleFrames),
		OPCODE(sfSetFrame),
		OPCODE(sfSetPortrait),
		OPCODE(sfSetProtagPortrait),
		OPCODE(sfChainBgdAnim),
		OPCODE(sfScriptSpecialWalk),
		OPCODE(sfPlaceActor),
		OPCODE(sfCheckUserInterrupt),
		OPCODE(sfScriptWalkRelative),
		OPCODE(sfScriptMoveRelative),
		OPCODE(sfSimulSpeech2),
		OPCODE(sfPsychicProfile),
		OPCODE(sfPsychicProfileOff),
		OPCODE(sfSetProtagState),
		OPCODE(sfResumeBgdAnim),
		OPCODE(sfThrowActor),
		OPCODE(sfWaitWalk),
		OPCODE(sfScriptSceneID),
		OPCODE(sfChangeActorScene),
		OPCODE(sfScriptClimb),
		OPCODE(sfSetDoorState),
		OPCODE(sfSetActorZ),
		OPCODE(sfScriptText),
		OPCODE(sfGetActorX),
		OPCODE(sfGetActorY),
		OPCODE(sfEraseDelta),
		OPCODE(sfPlayMusic),
		OPCODE(sfNull),
		OPCODE(sfEnableEscape),
		OPCODE(sfPlaySound),
		OPCODE(sfPlayLoopedSound),
		OPCODE(sfGetDeltaFrame),
		OPCODE(sfNull),
		OPCODE(sfNull),
		OPCODE(sfRand),
		OPCODE(sfFadeMusic),
		OPCODE(sfNull),
		OPCODE(sfSetChapterPoints),
		OPCODE(sfSetPortraitBgColor),
		OPCODE(sfScriptStartCutAway),
		OPCODE(sfReturnFromCutAway),
		OPCODE(sfEndCutAway),
		OPCODE(sfGetMouseClicks),
		OPCODE(sfResetMouseClicks),
		OPCODE(sfWaitFrames),
		OPCODE(sfScriptFade),
		OPCODE(sfScriptStartVideo),
		OPCODE(sfScriptReturnFromVideo),
		OPCODE(sfScriptEndVideo),
		OPCODE(sfSetActorZ),
		OPCODE(sfShowIHNMDemoHelpBg),
		OPCODE(sfAddIHNMDemoHelpTextLine),
		OPCODE(sfShowIHNMDemoHelpPage),
		OPCODE(sfVstopFX),
		OPCODE(sfVstopLoopedFX),
		OPCODE(sfDemoSetInteractive),	// only used in the demo version of IHNM
		OPCODE(sfDemoIsInteractive),
		OPCODE(sfVsetTrack),
		OPCODE(sfGetPoints),
		OPCODE(sfSetGlobalFlag),
		OPCODE(sfClearGlobalFlag),
		OPCODE(sfTestGlobalFlag),
		OPCODE(sfSetPoints),
		OPCODE(sfSetSpeechBox),
		OPCODE(sfDebugShowData),
		OPCODE(sfWaitFramesEsc),
		OPCODE(sfQueueMusic),
		OPCODE(sfDisableAbortSpeeches)
	};
	if (_vm->getGameType() == GType_IHNM)
		_scriptFunctionsList = IHNMscriptFunctionsList;
	else
		_scriptFunctionsList = ITEscriptFunctionsList;
}

// Script function #0 (0x00)
// Print a debugging message
void Script::sfPutString(SCRIPTFUNC_PARAMS) {
	const char *str;
	str = thread->_strings->getString(thread->pop());

	_vm->_console->DebugPrintf("sfPutString: %s\n",str);
	debug(0, "sfPutString: %s", str);
}

// Script function #1 (0x01) blocking
// Param1: time in ticks
void Script::sfWait(SCRIPTFUNC_PARAMS) {
	int16 time;
	time = thread->pop();

	if (!_skipSpeeches) {
		thread->waitDelay(_vm->ticksToMSec(time)); // put thread to sleep
	}
}

// Script function #2 (0x02)
void Script::sfTakeObject(SCRIPTFUNC_PARAMS) {
	uint16 objectId = thread->pop();
	ObjectData *obj;
	obj = _vm->_actor->getObj(objectId);
	if (obj->_sceneNumber != ITE_SCENE_INV) {
		obj->_sceneNumber = ITE_SCENE_INV;

		// WORKAROUND for two incorrect object sprites in the IHNM demo
		// (the mirror and the icon in Ted's part). Set them correctly here
		if (_vm->getGameId() == GID_IHNM_DEMO) {
			if (obj->_spriteListResourceId == 4 && objectId == 16408)
				obj->_spriteListResourceId = 24;
			if (obj->_spriteListResourceId == 3 && objectId == 16409)
				obj->_spriteListResourceId = 25;
		}

		_vm->_interface->addToInventory(objectId);
	}
}

// Script function #3 (0x03)
// Check if an object is carried.
void Script::sfIsCarried(SCRIPTFUNC_PARAMS) {
	uint16 objectId = thread->pop();
	CommonObjectData *object;
	if (_vm->_actor->validObjId(objectId)) {
		object = _vm->_actor->getObj(objectId);
		thread->_returnValue = (object->_sceneNumber == ITE_SCENE_INV) ? 1 : 0;
	} else {
		thread->_returnValue = 0;
	}


}

// Script function #4 (0x04) nonblocking
// Set the command display to the specified text string
// Param1: dialogue index of string
void Script::sfStatusBar(SCRIPTFUNC_PARAMS) {
	int16 stringIndex = thread->pop();

	_vm->_interface->setStatusText(thread->_strings->getString(stringIndex));
}

// Script function #5 (0x05)
void Script::sfMainMode(SCRIPTFUNC_PARAMS) {
	_vm->_actor->_centerActor = _vm->_actor->_protagonist;

	showVerb();
	_vm->_interface->activate();
	_vm->_interface->setMode(kPanelMain);

	if (_vm->getGameType() == GType_ITE)
		setPointerVerb();
}

// Script function #6 (0x06) blocking
// Param1: actor id
// Param2: actor x
// Param3: actor y
void Script::sfScriptWalkTo(SCRIPTFUNC_PARAMS) {
	uint16 actorId;
	Location actorLocation;
	ActorData *actor;

	actorId = thread->pop();
	actorLocation.x = thread->pop();
	actorLocation.y = thread->pop();

	actor = _vm->_actor->getActor(actorId);
	actorLocation.z = actor->_location.z;

	actor->_flags &= ~kFollower;

	if (_vm->_actor->actorWalkTo(actorId, actorLocation)) {
		thread->waitWalk(actor);
	}
}

// Script function #7 (0x07)
// Param1: actor id
// Param2: action
// Param3: theObject
// Param4: withObject
void Script::sfScriptDoAction(SCRIPTFUNC_PARAMS) {
	uint16 objectId;
	uint16 action;
	uint16 theObject;
	uint16 withObject;
	int16 scriptEntryPointNumber;
	int16 moduleNumber;
	ActorData *actor;
	ObjectData *obj;
	const HitZone *hitZone;
	Event event;

	objectId = thread->pop();
	action = thread->pop();
	theObject = thread->pop();
	withObject = thread->pop();

	switch (objectTypeId(objectId)) {
		case kGameObjectObject:
			obj = _vm->_actor->getObj(objectId);
			scriptEntryPointNumber = obj->_scriptEntrypointNumber;
			if (scriptEntryPointNumber <= 0) {
				return;
			}
			moduleNumber = 0;
			break;
		case kGameObjectActor:
			actor = _vm->_actor->getActor(objectId);
			scriptEntryPointNumber = actor->_scriptEntrypointNumber;
			if (scriptEntryPointNumber <= 0) {
				return;
			}
			if (actor->_flags & (kProtagonist | kFollower)) {
				moduleNumber = 0;
			} else {
				moduleNumber = _vm->_scene->getScriptModuleNumber();
			}
			break;
		case kGameObjectHitZone:
		case kGameObjectStepZone:
			if (objectTypeId(objectId) == kGameObjectHitZone)
				hitZone = _vm->_scene->_objectMap->getHitZone(objectIdToIndex(objectId));
			else
				hitZone = _vm->_scene->_actionMap->getHitZone(objectIdToIndex(objectId));

			if (hitZone == NULL)
				return;

			scriptEntryPointNumber = hitZone->getScriptNumber();
			moduleNumber = _vm->_scene->getScriptModuleNumber();
			break;
		default:
			error("Script::sfScriptDoAction wrong object type 0x%X", objectId);
	}

	event.type = kEvTOneshot;
	event.code = kScriptEvent;
	event.op = kEventExecNonBlocking;
	event.time = 0;
	event.param = moduleNumber;
	event.param2 = scriptEntryPointNumber;
	event.param3 = action;		// Action
	event.param4 = theObject;	// Object
	event.param5 = withObject;	// With Object
	event.param6 = objectId;

	_vm->_events->queue(&event);
}

// Script function #8 (0x08) nonblocking
// Param1: actor id
// Param2: actor orientation
void Script::sfSetActorFacing(SCRIPTFUNC_PARAMS) {
	int16 actorId;
	int actorDirection;
	ActorData *actor;

	actorId = thread->pop();
	actorDirection = thread->pop();

	actor = _vm->_actor->getActor(actorId);
	actor->_facingDirection = actor->_actionDirection = actorDirection;
	actor->_targetObject = ID_NOTHING;
}

// Script function #9 (0x09)
void Script::sfStartBgdAnim(SCRIPTFUNC_PARAMS) {
	int16 animId = thread->pop();
	int16 cycles = thread->pop();

	_vm->_anim->setCycles(animId, cycles);
	_vm->_anim->setFrameTime(animId, _vm->ticksToMSec(kRepeatSpeedTicks));
	_vm->_anim->play(animId, 0);

	debug(1, "sfStartBgdAnim(%d, %d)", animId, cycles);
}

// Script function #10 (0x0A)
void Script::sfStopBgdAnim(SCRIPTFUNC_PARAMS) {
	int16 animId = thread->pop();

	_vm->_anim->stop(animId);

	debug(1, "sfStopBgdAnim(%d)", animId);
}

// Script function #11 (0x0B) nonblocking
// If the parameter is true, the user interface is disabled while script
// continues to run. If the parameter is false, the user interface is
// reenabled.
// Param1: boolean
void Script::sfLockUser(SCRIPTFUNC_PARAMS) {
	int16 lock;

	lock = thread->pop();

	if (lock) {
		_vm->_interface->deactivate();
	} else {
		_vm->_interface->activate();
	}

}

// Script function #12 (0x0C)
// Disables mouse input, etc.
void Script::sfPreDialog(SCRIPTFUNC_PARAMS) {
	_vm->_interface->deactivate();
	_vm->_interface->converseClear();
	if (_vm->_interface->isInMainMode())
		_vm->_interface->setMode(kPanelConverse);
	else
		_vm->_interface->converseDisplayText();

	_vm->_interface->setMode(kPanelNull);
}

// Script function #13 (0x0D)
void Script::sfKillActorThreads(SCRIPTFUNC_PARAMS) {
	ScriptThread *anotherThread;
	ScriptThreadList::iterator threadIterator;
	int16 actorId;

	actorId = thread->pop();


	for (threadIterator = _threadList.begin(); threadIterator != _threadList.end(); ++threadIterator) {
		anotherThread = threadIterator.operator->();
		if ((anotherThread != thread) && (anotherThread->_threadVars[kThreadVarActor] == actorId)) {
			anotherThread->_flags &= ~kTFlagWaiting;
			anotherThread->_flags |= kTFlagAborted;
		}
	}
}

// Script function #14 (0x0E)
// Param1: actor id
// Param2: object id
void Script::sfFaceTowards(SCRIPTFUNC_PARAMS) {
	int16 actorId;
	int16 targetObject;
	ActorData *actor;

	actorId = thread->pop();
	targetObject = thread->pop();

	actor = _vm->_actor->getActor(actorId);
	actor->_targetObject = targetObject;
}

// Script function #15 (0x0F)
// Param1: actor id
// Param2: target object
void Script::sfSetFollower(SCRIPTFUNC_PARAMS) {
	int16 actorId;
	int16 targetObject;

	ActorData *actor;

	actorId = thread->pop();
	targetObject = thread->pop();

	debug(1, "sfSetFollower(%d, %d) [%d]", actorId, targetObject, _vm->_actor->actorIdToIndex(actorId));

	actor = _vm->_actor->getActor(actorId);
	actor->_targetObject = targetObject;
	if (targetObject != ID_NOTHING) {
		actor->_flags |= kFollower;
		actor->_actorFlags &= ~kActorNoFollow;
	} else {
		actor->_flags &= ~kFollower;
	}
}

// Script function #16 (0x10)
void Script::sfScriptGotoScene(SCRIPTFUNC_PARAMS) {
	int16 sceneNumber;
	int16 entrance;

	sceneNumber = thread->pop();
	entrance = thread->pop();
	if (_vm->getGameType() == GType_IHNM) {
		_vm->_gfx->setCursor(kCursorBusy);
	}

	if ((_vm->getGameType() == GType_ITE && sceneNumber < 0) ||
		(_vm->getGameType() == GType_IHNM && sceneNumber == 0)) {
		// TODO: set creditsFlag to true for IHNM
		_vm->shutDown();
		return;
	}

	if (_vm->getGameType() == GType_IHNM) {
		// WORKAROUND for the briefly appearing actors at the beginning of each chapter
		// This will stop the actors being drawn in those specific scenes until the scene background has been drawn
		if ((_vm->_scene->currentChapterNumber() == 1 && _vm->_scene->currentSceneNumber() == 6) ||
			(_vm->_scene->currentChapterNumber() == 2 && _vm->_scene->currentSceneNumber() == 31) ||
			(_vm->_scene->currentChapterNumber() == 3 && _vm->_scene->currentSceneNumber() == 58) ||
			(_vm->_scene->currentChapterNumber() == 4 && _vm->_scene->currentSceneNumber() == 68) ||
			(_vm->_scene->currentChapterNumber() == 5 && _vm->_scene->currentSceneNumber() == 91) ||
			(_vm->_scene->currentChapterNumber() == 7 && _vm->_scene->currentSceneNumber() == 145))
				_vm->_actor->showActors(false);		// Stop showing actors before the background is drawn

		// Since it doesn't look like the IHNM scripts remove the
		// cutaway after the intro, this is probably the best place to do it
		_vm->_anim->clearCutaway();
	}

	// It is possible to leave scene when converse panel is on,
	// particulalrly it may happen at Moneychanger tent. This
	// prevent this from happening.
	if (_vm->_interface->getMode() == kPanelConverse) {
		_vm->_interface->setMode(kPanelMain);
	}

	// changeScene calls loadScene which calls setVerb. setVerb resets all pending objects and object flags
	if (sceneNumber == -1 && _vm->getGameType() == GType_IHNM) {
		// Return back to the character selection screen in IHNM
		_vm->_scene->changeScene(154, entrance, kTransitionFade, 8);
	} else {
		_vm->_scene->changeScene(sceneNumber, entrance, (sceneNumber == ITE_SCENE_ENDCREDIT1) ? kTransitionFade : kTransitionNoFade);
	}

	if (_vm->_interface->getMode() == kPanelPlacard ||
		_vm->_interface->getMode() == kPanelCutaway ||
		_vm->_interface->getMode() == kPanelVideo) {
		_vm->_gfx->showCursor(true);
		_vm->_interface->setMode(kPanelMain);
	}

	_pendingVerb = _vm->_script->getVerbType(kVerbNone);
	_currentObject[0] = _currentObject[1] = ID_NOTHING;
	showVerb();	// calls setStatusText("")

	if (_vm->getGameType() == GType_IHNM)
		_vm->_gfx->setCursor(kCursorNormal);
}

// Script function #17 (0x11)
// Param1: object id
// Param2: sprite index
void Script::sfSetObjImage(SCRIPTFUNC_PARAMS) {
	uint16 objectId;
	uint16 spriteId;
	ObjectData *obj;

	objectId = thread->pop();
	spriteId = thread->pop();

	obj = _vm->_actor->getObj(objectId);
	if (_vm->getGameType() == GType_IHNM)
		obj->_spriteListResourceId = spriteId;
	else
		obj->_spriteListResourceId = OBJ_SPRITE_BASE + spriteId;
	_vm->_interface->refreshInventory();
}

// Script function #18 (0x12)
// Param1: object id
// Param2: name index
void Script::sfSetObjName(SCRIPTFUNC_PARAMS) {
	uint16 objectId;
	uint16 nameIdx;
	ObjectData *obj;

	objectId = thread->pop();
	nameIdx = thread->pop();

	obj = _vm->_actor->getObj(objectId);
	obj->_nameIndex = nameIdx;
}

// Script function #19 (0x13)
// Param1: object id
void Script::sfGetObjImage(SCRIPTFUNC_PARAMS) {
	uint16 objectId;
	ObjectData *obj;

	objectId = thread->pop();

	obj = _vm->_actor->getObj(objectId);

	if (_vm->getGameType() == GType_IHNM)
		thread->_returnValue = obj->_spriteListResourceId;
	else
		thread->_returnValue = obj->_spriteListResourceId - OBJ_SPRITE_BASE;
}

// Script function #20 (0x14)
void Script::sfGetNumber(SCRIPTFUNC_PARAMS) {
	if (_vm->_interface->_statusTextInputState == kStatusTextInputFirstRun) {
		_vm->_interface->enterStatusString();
		thread->wait(kWaitTypeStatusTextInput);
		disContinue = true;
	} else {
		if (_vm->_interface->_statusTextInputState == kStatusTextInputAborted) {
			thread->_returnValue = -1;
		} else {
			thread->_returnValue = atoi(_vm->_interface->_statusTextInputString);
		}

		_vm->_interface->_statusTextInputState = kStatusTextInputFirstRun;
	}
}

// Script function #21 (0x15)
// Param1: door #
void Script::sfScriptOpenDoor(SCRIPTFUNC_PARAMS) {
	int16 doorNumber;
	doorNumber = thread->pop();

	if (_vm->_scene->getFlags() & kSceneFlagISO) {
		_vm->_isoMap->setTileDoorState(doorNumber, 1);
	} else {
		_vm->_scene->setDoorState(doorNumber, 0);
	}
}

// Script function #22 (0x16)
// Param1: door #
void Script::sfScriptCloseDoor(SCRIPTFUNC_PARAMS) {
	int16 doorNumber;
	doorNumber = thread->pop();

	if (_vm->_scene->getFlags() & kSceneFlagISO) {
		_vm->_isoMap->setTileDoorState(doorNumber, 0);
	} else {
		_vm->_scene->setDoorState(doorNumber, 0xff);
	}
}

// Script function #23 (0x17)
void Script::sfSetBgdAnimSpeed(SCRIPTFUNC_PARAMS) {
	int16 animId = thread->pop();
	int16 speed = thread->pop();

	_vm->_anim->setFrameTime(animId, _vm->ticksToMSec(speed));
	debug(1, "sfSetBgdAnimSpeed(%d, %d)", animId, speed);
}

// Script function #24 (0x18)
void Script::SF_cycleColors(SCRIPTFUNC_PARAMS) {
	SF_stub("SF_cycleColors", thread, nArgs);

	error("Please, report this to sev");
}

// Script function #25 (0x19)
// Param1: actor id
void Script::sfDoCenterActor(SCRIPTFUNC_PARAMS) {
	int16 actorId;
	actorId = thread->pop();

	_vm->_actor->_centerActor = _vm->_actor->getActor(actorId);
}

// Script function #26 (0x1A) nonblocking
// Starts the specified animation
void Script::sfStartBgdAnimSpeed(SCRIPTFUNC_PARAMS) {
	int16 animId = thread->pop();
	int16 cycles = thread->pop();
	int16 speed = thread->pop();

	_vm->_anim->setCycles(animId, cycles);
	_vm->_anim->setFrameTime(animId, _vm->ticksToMSec(speed));
	_vm->_anim->play(animId, 0);

	debug(1, "sfStartBgdAnimSpeed(%d, %d, %d)", animId, cycles, speed);
}

// Script function #27 (0x1B) nonblocking
// Param1: actor id
// Param2: actor x
// Param3: actor y
void Script::sfScriptWalkToAsync(SCRIPTFUNC_PARAMS) {
	int16 actorId;
	Location actorLocation;
	ActorData *actor;

	actorId = thread->pop();
	actorLocation.x = thread->pop();
	actorLocation.y = thread->pop();

	actor = _vm->_actor->getActor(actorId);
	actorLocation.z = actor->_location.z;

	actor->_flags &= ~kFollower;

	_vm->_actor->actorWalkTo(actorId, actorLocation);
}

// Script function #28 (0x1C)
void Script::sfEnableZone(SCRIPTFUNC_PARAMS) {
	uint16 objectId = thread->pop();
	int16 flag = thread->pop();
	HitZone *hitZone;

	if (objectTypeId(objectId) == 0)
		return;
	else if (objectTypeId(objectId) == kGameObjectHitZone)
		hitZone = _vm->_scene->_objectMap->getHitZone(objectIdToIndex(objectId));
	else
		hitZone = _vm->_scene->_actionMap->getHitZone(objectIdToIndex(objectId));

	if (hitZone == NULL)
		return;

	if (flag) {
		hitZone->setFlag(kHitZoneEnabled);
	} else {
		hitZone->clearFlag(kHitZoneEnabled);
		_vm->_actor->_protagonist->_lastZone = NULL;
	}
}

// Script function #29 (0x1D)
// Param1: actor id
// Param2: current action
void Script::sfSetActorState(SCRIPTFUNC_PARAMS) {
	int16 actorId;
	int currentAction;
	ActorData *actor;

	actorId = thread->pop();
	currentAction = thread->pop();

	actor = _vm->_actor->getActor(actorId);

	if ((currentAction >= kActionWalkToPoint) && (currentAction <= kActionWalkToPoint)) {
		wakeUpActorThread(kWaitTypeWalk, actor);
	}
	actor->_currentAction = currentAction;
	actor->_actorFlags &= ~kActorBackwards;
}

// Script function #30 (0x1E) nonblocking
// Param1: actor id
// Param2: actor pos x
// Param3: actor pos y
void Script::sfScriptMoveTo(SCRIPTFUNC_PARAMS) {
	int16 objectId;
	Location location;
	ActorData *actor;
	ObjectData *obj;

	objectId = thread->pop();
	location.x = thread->pop();
	location.y = thread->pop();

	if (_vm->_actor->validActorId(objectId)) {
		actor = _vm->_actor->getActor(objectId);

		actor->_location.x = location.x;
		actor->_location.y = location.y;
	} else {
		if (_vm->_actor->validObjId(objectId)) {
			obj = _vm->_actor->getObj(objectId);
			obj->_location.x = location.x;
			obj->_location.y = location.y;
		}
	}
}

// Script function #31 (0x21)
// Param1: sceneNumber
void Script::sfSceneEq(SCRIPTFUNC_PARAMS) {
	int16 sceneNumber = thread->pop();

	if (_vm->_scene->getSceneResourceId(sceneNumber) == _vm->_scene->currentSceneResourceId())
		thread->_returnValue = 1;
	else
		thread->_returnValue = 0;
}

// Script function #32 (0x20)
void Script::sfDropObject(SCRIPTFUNC_PARAMS) {
	uint16 objectId;
	uint16 spriteId;
	int16 x;
	int16 y;
	ObjectData *obj;

	objectId = thread->pop();
	spriteId = thread->pop();
	x = thread->pop();
	y = thread->pop();

	obj = _vm->_actor->getObj(objectId);

	if (obj->_sceneNumber == ITE_SCENE_INV) {
		_vm->_interface->removeFromInventory(objectId);
	}

	obj->_sceneNumber = _vm->_scene->currentSceneNumber();

	// HACK for the compact disk in Ellen's chapter
	// Change the scene number of the compact disk so that it's not shown. It will be shown 
	// once Ellen says that there's something different (i.e. after speaking with AM)
	// See Actor::actorSpeech for the other part of this hack
	if (_vm->getGameType() == GType_IHNM && _vm->_scene->currentChapterNumber() == 3 &&
		_vm->_scene->currentSceneNumber() == 59 && obj->_id == 16385)
			obj->_sceneNumber = -1;

	if (_vm->getGameType() == GType_IHNM) {
		if (_vm->getGameId() != GID_IHNM_DEMO) {
			obj->_spriteListResourceId = spriteId;
		} else {
			// Don't update the object's _spriteListResourceId in the IHNM demo, as this function is
			// called incorrectly there (with spriteId == 0, which resets the object sprites)
		}
	} else {
		obj->_spriteListResourceId = OBJ_SPRITE_BASE + spriteId;
	}

	obj->_location.x = x;
	obj->_location.y = y;
}

// Script function #33 (0x21)
void Script::sfFinishBgdAnim(SCRIPTFUNC_PARAMS) {
	int16 animId = thread->pop();

	_vm->_anim->finish(animId);

	debug(1, "sfFinishBgdAnim(%d)", animId);
}

// Script function #34 (0x22)
// Param1: actor id 1
// Param2: actor id 2
void Script::sfSwapActors(SCRIPTFUNC_PARAMS) {
	int16 actorId1;
	int16 actorId2;
	ActorData *actor1;
	ActorData *actor2;

	actorId1 = thread->pop();
	actorId2 = thread->pop();

	actor1 = _vm->_actor->getActor(actorId1);
	actor2 = _vm->_actor->getActor(actorId2);

	SWAP(actor1->_location, actor2->_location);

	if (actor1->_flags & kProtagonist) {
		actor1->_flags &= ~kProtagonist;
		actor2->_flags |= kProtagonist;
		_vm->_actor->_protagonist = _vm->_actor->_centerActor = actor2;
		if (_vm->getGameType() == GType_IHNM)
			_vm->_scene->setProtag(actorId2);
	} else if (actor2->_flags & kProtagonist) {
		actor2->_flags &= ~kProtagonist;
		actor1->_flags |= kProtagonist;
		_vm->_actor->_protagonist = _vm->_actor->_centerActor = actor1;
		if (_vm->getGameType() == GType_IHNM)
			_vm->_scene->setProtag(actorId1);
	}
}

// Script function #35 (0x23)
// Param1: string rid
// Param2: actorscount
// Param3: actor id1
///....
// Param3: actor idN
void Script::sfSimulSpeech(SCRIPTFUNC_PARAMS) {
	int16 stringId;
	int16 actorsCount;
	int i;
	uint16 actorsIds[ACTOR_SPEECH_ACTORS_MAX];
	const char *string;
	int16 sampleResourceId = -1;

	stringId = thread->pop();
	actorsCount = thread->pop();

	if (actorsCount > ACTOR_SPEECH_ACTORS_MAX)
		error("sfSimulSpeech actorsCount=0x%X exceed ACTOR_SPEECH_ACTORS_MAX", actorsCount);

	for (i = 0; i < actorsCount; i++)
		actorsIds[i] = thread->pop();

	string = thread->_strings->getString(stringId);

	if (thread->_voiceLUT->voices) {
		if (_vm->getGameType() == GType_IHNM && stringId >= 338) {
			sampleResourceId = -1;
		} else {
			sampleResourceId = thread->_voiceLUT->voices[stringId];
			if (sampleResourceId <= 0 || sampleResourceId > 4000)
				sampleResourceId = -1;
		}
	}

	_vm->_actor->simulSpeech(string, actorsIds, actorsCount, 0, sampleResourceId);
	thread->wait(kWaitTypeSpeech);
}

// Script function #36 (0x24) ?
// Param1: actor id
// Param2: actor x
// Param3: actor y
// Param4: actor walk flag
void Script::sfScriptWalk(SCRIPTFUNC_PARAMS) {
	int16 actorId;
	Location actorLocation;
	ActorData *actor;
	uint16 walkFlags;

	actorId = thread->pop();
	actorLocation.x = thread->pop();
	actorLocation.y = thread->pop();
	walkFlags = thread->pop();

	actor = _vm->_actor->getActor(actorId);
	actorLocation.z = actor->_location.z;

	_vm->_actor->realLocation(actorLocation, ID_NOTHING, walkFlags);

	actor->_flags &= ~kFollower;

	if (_vm->_actor->actorWalkTo(actorId, actorLocation) && !(walkFlags & kWalkAsync)) {
		thread->waitWalk(actor);
	}

	if (walkFlags & kWalkBackPedal) {
		actor->_actorFlags |= kActorBackwards;
	}

	actor->_actorFlags = (actor->_actorFlags & ~kActorFacingMask) | (walkFlags & kActorFacingMask);
}

// Script function #37 (0x25) nonblocking
// Param1: actor id
// Param2: flags telling how to cycle the frames
// Param3: cycle frame number
// Param4: cycle delay
void Script::sfCycleFrames(SCRIPTFUNC_PARAMS) {
	int16 actorId;
	int16 flags;
	int cycleFrameSequence;
	int cycleDelay;
	ActorData *actor;

	actorId = thread->pop();
	flags = thread->pop();
	cycleFrameSequence = thread->pop();
	cycleDelay = thread->pop();

	actor = _vm->_actor->getActor(actorId);

	if (flags & kCyclePong) {
		actor->_currentAction = kActionPongFrames;
	} else {
		actor->_currentAction = kActionCycleFrames;
	}

	actor->_actorFlags &= ~(kActorContinuous | kActorRandom | kActorBackwards);

	if (!(flags & kCycleOnce)) {
		actor->_actorFlags |= kActorContinuous;
	}
	if (flags & kCycleRandom) {
		actor->_actorFlags |= kActorRandom;
	}
	if (flags & kCycleReverse) {
		if (_vm->getGameType() == GType_IHNM && 
			_vm->_scene->currentChapterNumber() == 2 && _vm->_scene->currentSceneNumber() == 41) {
			// Prevent Benny from walking backwards after talking to the child via the monitor. This occurs in the
			// original as well, and is fixed by not setting the kActorBackwards flag at this point
		} else {
			actor->_actorFlags |= kActorBackwards;
		}
	}

	actor->_cycleFrameSequence = cycleFrameSequence;
	actor->_cycleTimeCount = 0;
	actor->_cycleDelay = cycleDelay;
	actor->_actionCycle = 0;
}

// Script function #38 (0x26) nonblocking
// Param1: actor id
// Param2: frame type
// Param3: frame offset
void Script::sfSetFrame(SCRIPTFUNC_PARAMS) {
	int16 actorId;
	int frameType;
	int frameOffset;
	ActorData *actor;
	ActorFrameRange *frameRange;

	actorId = thread->pop();
	frameType = thread->pop();
	frameOffset = thread->pop();

	actor = _vm->_actor->getActor(actorId);

	frameRange = _vm->_actor->getActorFrameRange(actorId, frameType);

	actor->_frameNumber = frameRange->frameIndex + frameOffset;

	if (actor->_currentAction != kActionFall) {
		actor->_currentAction = kActionFreeze;
	}
}

// Script function #39 (0x27)
// Sets the right-hand portrait
void Script::sfSetPortrait(SCRIPTFUNC_PARAMS) {
	int16 param = thread->pop();

	_vm->_interface->setRightPortrait(param);
}

// Script function #40 (0x28)
// Sets the left-hand portrait
void Script::sfSetProtagPortrait(SCRIPTFUNC_PARAMS) {
	int16 param = thread->pop();

	_vm->_interface->setLeftPortrait(param);
}

// Script function #41 (0x29) nonblocking
// Links the specified animations for playback

// Param1: ?
// Param2: total linked frame count
// Param3: animation id link target
// Param4: animation id link source
void Script::sfChainBgdAnim(SCRIPTFUNC_PARAMS) {
	int16 animId1 = thread->pop();
	int16 animId = thread->pop();
	int16 cycles = thread->pop();
	int16 speed = thread->pop();

	if (speed >= 0) {
		_vm->_anim->setCycles(animId, cycles);
		_vm->_anim->stop(animId);
		_vm->_anim->setFrameTime(animId, _vm->ticksToMSec(speed));
	}

	_vm->_anim->link(animId1, animId);
	debug(1, "sfChainBgdAnim(%d, %d, %d, %d)", animId1, animId, cycles, speed);
}

// Script function #42 (0x2A)
// Param1: actor id
// Param2: actor x
// Param3: actor y
// Param4: frame seq
void Script::sfScriptSpecialWalk(SCRIPTFUNC_PARAMS) {
	int16 actorId;
	int16 walkFrameSequence;
	Location actorLocation;
	ActorData *actor;

	actorId = thread->pop();
	actorLocation.x = thread->pop();
	actorLocation.y = thread->pop();
	walkFrameSequence = thread->pop();

	actor = _vm->_actor->getActor(actorId);
	actorLocation.z = actor->_location.z;

	_vm->_actor->actorWalkTo(actorId, actorLocation);

	actor->_walkFrameSequence = walkFrameSequence;
}

// Script function #43 (0x2B) nonblocking
// Param1: actor id
// Param2: actor x
// Param3: actor y
// Param4: actor direction
// Param5: actor action
// Param6: actor frame number
void Script::sfPlaceActor(SCRIPTFUNC_PARAMS) {
	int16 actorId;
	Location actorLocation;
	int actorDirection;
	int frameType;
	int frameOffset;
	ActorData *actor;
	ActorFrameRange *frameRange;

	actorId = thread->pop();
	actorLocation.x = thread->pop();
	actorLocation.y = thread->pop();
	actorDirection = thread->pop();
	frameType = thread->pop();
	frameOffset = thread->pop();

	debug(1, "sfPlaceActor(id = 0x%x, x=%d, y=%d, dir=%d, frameType=%d, frameOffset=%d)", actorId, actorLocation.x,
		  actorLocation.y, actorDirection, frameType, frameOffset);

	actor = _vm->_actor->getActor(actorId);
	actor->_location.x = actorLocation.x;
	actor->_location.y = actorLocation.y;
	actor->_facingDirection = actor->_actionDirection = actorDirection;

	if (!actor->_frames)
		_vm->_actor->loadActorResources(actor); //? is not it already loaded ?

	if (frameType >= 0) {
		frameRange = _vm->_actor->getActorFrameRange(actorId, frameType);

		if (frameRange->frameCount <= frameOffset) {
			warning("Wrong frameOffset 0x%X", frameOffset);
		}

		actor->_frameNumber = frameRange->frameIndex + frameOffset;
		actor->_currentAction = kActionFreeze;
	} else {
		actor->_currentAction = kActionWait;
	}

	actor->_targetObject = ID_NOTHING;
}

// Script function #44 (0x2C) nonblocking
// Checks to see if the user has interrupted a currently playing
// game cinematic. Pushes a zero or positive value if the game
// has not been interrupted.
void Script::sfCheckUserInterrupt(SCRIPTFUNC_PARAMS) {
	thread->_returnValue = (_skipSpeeches == true);
}

// Script function #45 (0x2D)
// Param1: actor id
// Param2: object id
// Param3: actor x
// Param4: actor y
// Param5: actor walk flag
void Script::sfScriptWalkRelative(SCRIPTFUNC_PARAMS) {
	int16 actorId;
	int16 objectId;
	uint16 walkFlags;
	Location actorLocation;
	ActorData *actor;

	actorId = thread->pop();
	objectId = thread->pop();
	actorLocation.x = thread->pop();
	actorLocation.y = thread->pop();
	walkFlags = thread->pop();

	actor = _vm->_actor->getActor(actorId);
	actorLocation.z = actor->_location.z;

	_vm->_actor->realLocation(actorLocation, objectId, walkFlags);

	actor->_flags &= ~kFollower;

	if (_vm->_actor->actorWalkTo(actorId, actorLocation) && !(walkFlags & kWalkAsync)) {
		thread->waitWalk(actor);
	}

	if (walkFlags & kWalkBackPedal) {
		actor->_actorFlags |= kActorBackwards;
	}

	actor->_actorFlags = (actor->_actorFlags & ~kActorFacingMask) | (walkFlags & kActorFacingMask);
}

// Script function #46 (0x2E)
// Param1: actor id
// Param2: object id
// Param3: actor x
// Param4: actor y
// Param5: actor walk flag
void Script::sfScriptMoveRelative(SCRIPTFUNC_PARAMS) {
	int16 actorId;
	int16 objectId;
	uint16 walkFlags;
	Location actorLocation;
	ActorData *actor;

	actorId = thread->pop();
	objectId = thread->pop();
	actorLocation.x = thread->pop();
	actorLocation.y = thread->pop();
	walkFlags = thread->pop();

	actor = _vm->_actor->getActor(actorId);
	actorLocation.z = actor->_location.z;

	_vm->_actor->realLocation(actorLocation, objectId, walkFlags);


	actor->_location = actorLocation;
	actor->_actorFlags = (actor->_actorFlags & ~kActorFacingMask) | (walkFlags & kActorFacingMask);
}

// Script function #47 (0x2F)
void Script::sfSimulSpeech2(SCRIPTFUNC_PARAMS) {
	int16 stringId;
	int16 actorsCount;
	int16 speechFlags;
	int i;
	uint16 actorsIds[ACTOR_SPEECH_ACTORS_MAX];
	const char *string;
	int16 sampleResourceId = -1;

	stringId = thread->pop();
	actorsCount = thread->pop();
	speechFlags = thread->pop();

	if (actorsCount > ACTOR_SPEECH_ACTORS_MAX)
		error("sfSimulSpeech2 actorsCount=0x%X exceed ACTOR_SPEECH_ACTORS_MAX", actorsCount);

	for (i = 0; i < actorsCount; i++)
		actorsIds[i] = thread->pop();

	string = thread->_strings->getString(stringId);

	if (thread->_voiceLUT->voices) {
		sampleResourceId = thread->_voiceLUT->voices[stringId];
		if (sampleResourceId <= 0 || sampleResourceId > 4000)
			sampleResourceId = -1;
	}

	_vm->_actor->simulSpeech(string, actorsIds, actorsCount, speechFlags, sampleResourceId);
	thread->wait(kWaitTypeSpeech);
}


// Script function #48 (0x30)
// Param1: string rid
void Script::sfPlacard(SCRIPTFUNC_PARAMS) {
	int stringId;
	Surface *backBuffer = _vm->_gfx->getBackBuffer();
	static PalEntry cur_pal[PAL_ENTRIES];
	PalEntry *pal;
	Event event;
	Event *q_event;

	thread->wait(kWaitTypePlacard);

	_vm->_interface->rememberMode();
	_vm->_interface->setMode(kPanelPlacard);

	stringId = thread->pop();

	event.type = kEvTOneshot;
	event.code = kCursorEvent;
	event.op = kEventHide;

	q_event = _vm->_events->queue(&event);

	_vm->_gfx->getCurrentPal(cur_pal);

	event.type = kEvTImmediate;
	event.code = kPalEvent;
	event.op = kEventPalToBlack;
	event.time = 0;
	event.duration = kNormalFadeDuration;
	event.data = cur_pal;

	q_event = _vm->_events->chain(q_event, &event);

	event.type = kEvTOneshot;
	event.code = kInterfaceEvent;
	event.op = kEventClearStatus;

	q_event = _vm->_events->chain(q_event, &event);

	event.type = kEvTOneshot;
	event.code = kGraphicsEvent;
	event.op = kEventSetFlag;
	event.param = RF_PLACARD;

	q_event = _vm->_events->chain(q_event, &event);

	event.type = kEvTOneshot;
	event.code = kGraphicsEvent;
	event.op = kEventFillRect;
	event.data = backBuffer;
	event.param = 138;
	event.param2 = 0;
	event.param3 = _vm->_scene->getHeight();
	event.param4 = 0;
	event.param5 = _vm->getDisplayWidth();

	q_event = _vm->_events->chain(q_event, &event);

	// Put the text in the center of the viewport, assuming it will fit on
	// one line. If we cannot make that assumption we'll need to extend
	// the text drawing function so that it can center text around a point.
	// It doesn't end up in exactly the same spot as the original did it,
	// but it's close enough for now at least.

	TextListEntry textEntry;

	textEntry.knownColor = kKnownColorBrightWhite;
	textEntry.effectKnownColor = kKnownColorBlack;
	textEntry.point.x = _vm->getDisplayWidth() / 2;
	textEntry.point.y = (_vm->_scene->getHeight() - _vm->_font->getHeight(kKnownFontMedium)) / 2;
	textEntry.font = kKnownFontMedium;
	textEntry.flags = (FontEffectFlags)(kFontOutline | kFontCentered);
	textEntry.text = thread->_strings->getString(stringId);

	_placardTextEntry = _vm->_scene->_textList.addEntry(textEntry);

	event.type = kEvTOneshot;
	event.code = kTextEvent;
	event.op = kEventDisplay;
	event.data = _placardTextEntry;

	q_event = _vm->_events->chain(q_event, &event);

	_vm->_scene->getBGPal(pal);

	event.type = kEvTImmediate;
	event.code = kPalEvent;
	event.op = kEventBlackToPal;
	event.time = 0;
	event.duration = kNormalFadeDuration;
	event.data = pal;

	q_event = _vm->_events->chain(q_event, &event);

	event.type = kEvTOneshot;
	event.code = kScriptEvent;
	event.op = kEventThreadWake;
	event.param = kWaitTypePlacard;

	q_event = _vm->_events->chain(q_event, &event);

}

// Script function #49 (0x31)
void Script::sfPlacardOff(SCRIPTFUNC_PARAMS) {
	thread->wait(kWaitTypePlacard);

	_vm->_scene->clearPlacard();
}

void Script::sfPsychicProfile(SCRIPTFUNC_PARAMS) {
	thread->wait(kWaitTypePlacard);

	int stringId = thread->pop();
	_vm->_scene->showPsychicProfile(thread->_strings->getString(stringId));
}

void Script::sfPsychicProfileOff(SCRIPTFUNC_PARAMS) {
	thread->wait(kWaitTypePlacard);

	// This is called a while after the psychic profile is
	// opened, to close it automatically
	_vm->_scene->clearPsychicProfile();
}

// Script function #50 (0x32)
void Script::sfSetProtagState(SCRIPTFUNC_PARAMS) {
	int protagState = thread->pop();

	_vm->_actor->setProtagState(protagState);
}

// Script function #51 (0x33)
void Script::sfResumeBgdAnim(SCRIPTFUNC_PARAMS) {
	int16 animId = thread->pop();
	int16 cycles = thread->pop();

	_vm->_anim->resume(animId, cycles);
	debug(1, "sfResumeBgdAnimSpeed(%d, %d)", animId, cycles);

}

// Script function #52 (0x34)
// Param1: actor id
// Param2: x
// Param3: y
// Param4: unknown
// Param5: actionCycle
// Param6: flags
void Script::sfThrowActor(SCRIPTFUNC_PARAMS) {
	int16 actorId;
	ActorData *actor;
	int16 flags;
	int32 actionCycle;
	Location location;

	actorId = thread->pop();
	location.x = thread->pop();
	location.y = thread->pop();
	thread->pop();
	actionCycle = thread->pop();
	flags = thread->pop();

	actor = _vm->_actor->getActor(actorId);
	location.z = actor->_location.z;
	actor->_currentAction = kActionFall;
	actor->_actionCycle = actionCycle;
	actor->_fallAcceleration	= -20;
	actor->_fallVelocity = - (actor->_fallAcceleration * actor->_actionCycle) / 2;
	actor->_fallPosition	= actor->_location.z << 4;

	actor->_finalTarget = location;
	actor->_actionCycle--;
	if (!(flags & kWalkAsync)) {
		thread->waitWalk(actor);
	}
}

// Script function #53 (0x35)
// Param1: actor id
// Param2: target object
void Script::sfWaitWalk(SCRIPTFUNC_PARAMS) {
	int16 actorId;
	ActorData *actor;

	actorId = thread->pop();
	actor = _vm->_actor->getActor(actorId);

	if ((actor->_currentAction == kActionWalkToPoint) ||
		(actor->_currentAction == kActionWalkToLink) ||
		(actor->_currentAction == kActionFall)) {
			thread->waitWalk(actor);
	}
}

// Script function #54 (0x36)
void Script::sfScriptSceneID(SCRIPTFUNC_PARAMS) {
	thread->_returnValue = _vm->_scene->currentSceneNumber();
}

// Script function #55 (0x37)
// Param1: actor id
// Param2: scene number
void Script::sfChangeActorScene(SCRIPTFUNC_PARAMS) {
	int16 actorId;
	int32 sceneNumber;
	ActorData *actor;

	actorId = thread->pop();
	sceneNumber = thread->pop();
	actor = _vm->_actor->getActor(actorId);
	actor->_sceneNumber = sceneNumber;
}

// Script function #56 (0x38)
// Param1: actor id
// Param2: z
// Param3: frame seq
// Param4: flags
void Script::sfScriptClimb(SCRIPTFUNC_PARAMS) {
	int16 actorId;
	int16 z;
	ActorData *actor;
	uint16 flags;
	int cycleFrameSequence;

	actorId = thread->pop();
	z = thread->pop();
	cycleFrameSequence = thread->pop();
	flags = thread->pop();

	actor = _vm->_actor->getActor(actorId);
	actor->_finalTarget.z = z;
	actor->_flags &= ~kFollower;
	actor->_actionCycle = 1;
	actor->_cycleFrameSequence = cycleFrameSequence;
	actor->_currentAction = kActionClimb;
	if (!(flags & kWalkAsync)) {
		thread->waitWalk(actor);
	}
}

// Script function #57 (0x39)
// Param1: door #
// Param2: door state
void Script::sfSetDoorState(SCRIPTFUNC_PARAMS) {
	int16 doorNumber;
	int16 doorState;
	doorNumber = thread->pop();
	doorState = thread->pop();

	if (_vm->_scene->getFlags() & kSceneFlagISO) {
		_vm->_isoMap->setTileDoorState(doorNumber, doorState);
	} else {
		_vm->_scene->setDoorState(doorNumber, doorState);
	}
}

// Script function #58 (0x3A)
// Param1: actor id
// Param2: z
void Script::sfSetActorZ(SCRIPTFUNC_PARAMS) {
	int16 objectId;
	ActorData *actor;
	ObjectData *obj;
	int16 z;

	objectId = thread->pop();
	z = thread->pop();


	if (_vm->_actor->validActorId(objectId)) {
		actor = _vm->_actor->getActor(objectId);
		actor->_location.z = z;
	} else {
		if (_vm->_actor->validObjId(objectId)) {
			obj = _vm->_actor->getObj(objectId);
			obj->_location.z = z;
		}
	}
}

// Script function #59 (0x3B)
// Param1: stringId
// Param2: flags
// Param3: color
// Param4: x
// Param5: y
void Script::sfScriptText(SCRIPTFUNC_PARAMS) {
	int16 stringId;
	int16 flags;
	Rect rect;
	int color;
	Point point;
	int width;
	const char*text;
	stringId = thread->pop();
	flags = thread->pop();
	color = thread->pop();
	point.x = thread->pop();
	point.y = thread->pop();

	text = thread->_strings->getString(stringId);

	width = _vm->_font->getStringWidth(kKnownFontScript, text, 0, kFontOutline);
	rect.top = point.y - 6;
	rect.setHeight(12);
	rect.left = point.x - width / 2;
	rect.setWidth(width);

	_vm->_actor->setSpeechColor(color, _vm->KnownColor2ColorId(kKnownColorBlack));
	_vm->_actor->nonActorSpeech(rect, &text, 1, -1, flags);
}

// Script function #60 (0x3C)
// Param1: actor id
void Script::sfGetActorX(SCRIPTFUNC_PARAMS) {
	int16 actorId;
	ActorData *actor;

	actorId = thread->pop();
	actor = _vm->_actor->getActor(actorId);

	thread->_returnValue = actor->_location.x >> 2;
}

// Script function #61 (0x3D)
// Param1: actor id
void Script::sfGetActorY(SCRIPTFUNC_PARAMS) {
	int16 actorId;
	ActorData *actor;

	actorId = thread->pop();
	actor = _vm->_actor->getActor(actorId);

	thread->_returnValue = actor->_location.y >> 2;
}

// Script function #62 (0x3E)
void Script::sfEraseDelta(SCRIPTFUNC_PARAMS) {
	Surface *backGroundSurface;
	BGInfo backGroundInfo;

	backGroundSurface = _vm->_render->getBackGroundSurface();
	_vm->_scene->getBGInfo(backGroundInfo);

	backGroundSurface->blit(backGroundInfo.bounds, backGroundInfo.buffer);
}

// Script function #63 (0x3F)
void Script::sfPlayMusic(SCRIPTFUNC_PARAMS) {
	if (_vm->getGameType() == GType_ITE) {
		int16 param = thread->pop() + 9;

		if (param >= 9 && param <= 34) {
			_vm->_music->setVolume(_vm->_musicVolume == 10 ? -1 : _vm->_musicVolume * 25, 1);
			_vm->_music->play(param);
		} else {
			_vm->_music->stop();
		}
	} else {
		int16 param1 = thread->pop();
		int16 param2 = thread->pop();

		if (param1 < 0) {
			_vm->_music->stop();
			return;
		}

		if (param1 >= _vm->_music->_songTableLen) {
			warning("sfPlayMusic: Wrong song number (%d > %d)", param1, _vm->_music->_songTableLen - 1);
		} else {
			_vm->_music->setVolume(_vm->_musicVolume == 10 ? -1 : _vm->_musicVolume * 25, 1);
			_vm->_music->play(_vm->_music->_songTable[param1], param2 ? MUSIC_LOOP : MUSIC_NORMAL);
			if (!_vm->_scene->haveChapterPointsChanged()) {
				_vm->_scene->setCurrentMusicTrack(param1);
				_vm->_scene->setCurrentMusicRepeat(param2);
			} else {
				// Don't save this music track when saving in IHNM
				_vm->_scene->setChapterPointsChanged(false);
			}
		}
	}
}

// Script function #64 (0x40)
void Script::sfPickClimbOutPos(SCRIPTFUNC_PARAMS) {
	int16 u, v, t;
	ActorData *protagonist = _vm->_actor->_protagonist;
	while (true) {

		u = (_vm->_rnd.getRandomNumber(63) & 63) + 40;
		v = (_vm->_rnd.getRandomNumber(63) & 63) + 40;
		t = _vm->_isoMap->getTileIndex(u, v, 6);
		if (t == 65) {
			protagonist->_location.u() = (u << 4) + 4;
			protagonist->_location.v() = (v << 4) + 4;
			protagonist->_location.z = 48;
			break;
		}

	}
}

// Script function #65 (0x41)
void Script::sfTossRif(SCRIPTFUNC_PARAMS) {
	int16 uc , vc;
	uint16 direction;
	ActorData *protagonist = _vm->_actor->_protagonist;

	uc = protagonist->_location.u() >> 4;
	vc = protagonist->_location.v() >> 4;
	if (_vm->_isoMap->findNearestChasm(uc, vc, direction)) {
		uc <<= 4;
		vc <<= 4;
		protagonist->_facingDirection = direction;

		protagonist->_finalTarget.u() = uc;
		protagonist->_finalTarget.v() = vc;
		protagonist->_finalTarget.z = -40;
		protagonist->_currentAction = kActionFall;
		protagonist->_actionCycle = 24;
		protagonist->_fallAcceleration = - 20;
		protagonist->_fallVelocity = - (protagonist->_fallAcceleration * 16) / 2 - (44 / 12);
		protagonist->_fallPosition = protagonist->_location.z << 4;
		protagonist->_actionCycle--;
	}
}

// Script function #66 (0x42)
void Script::sfShowControls(SCRIPTFUNC_PARAMS) {
	// It has zero implementation in Win rerelase, and in DOS
	// release it deals with video ports.
}

// Script function #67 (0x43)
void Script::sfShowMap(SCRIPTFUNC_PARAMS) {
	_vm->_interface->setMode(kPanelMap);
}

// Script function #68 (0x44)
void Script::sfPuzzleWon(SCRIPTFUNC_PARAMS) {
	thread->_returnValue = _vm->_puzzle->isSolved();
}

// Script function #69 (0x45)
void Script::sfEnableEscape(SCRIPTFUNC_PARAMS) {
	if (thread->pop())
		_abortEnabled = true;
	else {
		_skipSpeeches = false;
		_abortEnabled = false;
	}
}

// Script function #70 (0x46)
void Script::sfPlaySound(SCRIPTFUNC_PARAMS) {
	int16 param = thread->pop();
	int res;

	if (param >= 0 && param < _vm->_sndRes->_fxTableLen) {
		res = _vm->_sndRes->_fxTable[param].res;
		if (_vm->getFeatures() & GF_CD_FX)
			res -= 14;
		_vm->_sndRes->playSound(res, _vm->_sndRes->_fxTable[param].vol, false);
	} else {
		_vm->_sound->stopSound();
	}
}

// Script function #71 (0x47)
void Script::sfPlayLoopedSound(SCRIPTFUNC_PARAMS) {
	int16 param = thread->pop();
	int res;

	if (param >= 0 && param < _vm->_sndRes->_fxTableLen) {
		res = _vm->_sndRes->_fxTable[param].res;
		if (_vm->getFeatures() & GF_CD_FX)
			res -= 14;

		_vm->_sndRes->playSound(res, _vm->_sndRes->_fxTable[param].vol, true);
	} else {
		_vm->_sound->stopSound();
	}
}

// Script function #72 (0x48)
void Script::sfGetDeltaFrame(SCRIPTFUNC_PARAMS) {
	uint16 animId = (uint16)thread->pop();

	thread->_returnValue = _vm->_anim->getCurrentFrame(animId);
}

// Script function #73 (0x49)
void Script::sfShowProtect(SCRIPTFUNC_PARAMS) {
	if (_vm->_copyProtection) {
		thread->wait(kWaitTypeRequest);

		_vm->_interface->setMode(kPanelProtect);
	}
}

// Script function #74 (0x4A)
void Script::sfProtectResult(SCRIPTFUNC_PARAMS) {
	if (_vm->_copyProtection) {
		thread->_returnValue = _vm->_interface->getProtectHash();
	} else {
		int protectHash;

		//cheating
		protectHash = thread->pop();
		thread->push(protectHash);
		thread->_returnValue = protectHash;
	}
}

// Script function #75 (0x4b)
void Script::sfRand(SCRIPTFUNC_PARAMS) {
	int16 param;

	param = thread->pop();
	thread->_returnValue = _vm->_rnd.getRandomNumber(param - 1);
}

// Script function #76 (0x4c)
void Script::sfFadeMusic(SCRIPTFUNC_PARAMS) {
	_vm->_music->setVolume(0, 1000);
}

// Script function #77 (0x4d)
void Script::sfPlayVoice(SCRIPTFUNC_PARAMS) {
	int16 param = thread->pop();

	warning("sfPlayVoice(%d)", param);
	if (param > 0) {
		_vm->_sndRes->playVoice(param + 3712);
	} else {
		_vm->_sound->stopSound();
	}
}

void Script::finishDialog(int strID, int replyID, int flags, int bitOffset) {
	byte *addr;
	const char *str;

	if (_conversingThread) {
		_vm->_interface->setMode(kPanelNull);

		if (_vm->getGameType() == GType_IHNM) {
			str = _conversingThread->_strings->getString(strID);
			if (*str != '[') {
				int sampleResourceId = -1;
				sampleResourceId = _conversingThread->_voiceLUT->voices[strID];
				if (sampleResourceId < 0 || sampleResourceId > 4000)
					sampleResourceId = -1;

				_vm->_actor->actorSpeech(_vm->_actor->_protagonist->_id, &str, 1, sampleResourceId, 0);
			}
		}

		_conversingThread->_flags &= ~kTFlagWaiting;

		_conversingThread->push(replyID);

		if (flags & kReplyOnce) {
			addr = _conversingThread->_staticBase + (bitOffset >> 3);
			*addr |= (1 << (bitOffset & 7));
		}
	}

	_conversingThread = NULL;
	wakeUpThreads(kWaitTypeDialogBegin);
}

void Script::sfSetChapterPoints(SCRIPTFUNC_PARAMS) {
	int16 ethics = thread->pop();
	int16 barometer = thread->pop();
	int chapter = _vm->_scene->currentChapterNumber();

	_vm->_ethicsPoints[chapter] = ethics;
	_vm->_spiritualBarometer = ethics * 256 / barometer;
	_vm->_scene->setChapterPointsChanged(true);		// don't save this music when saving in IHNM

	if (_vm->_spiritualBarometer > 255)
		_vm->_gfx->setPaletteColor(kIHNMColorPortrait, 0xff, 0xff, 0xff);
	else
		_vm->_gfx->setPaletteColor(kIHNMColorPortrait,
			_vm->_spiritualBarometer * _vm->_interface->_portraitBgColor.red / 256,
			_vm->_spiritualBarometer * _vm->_interface->_portraitBgColor.green / 256,
			_vm->_spiritualBarometer * _vm->_interface->_portraitBgColor.blue / 256);

	PalEntry *palPointer;
	_vm->_scene->getBGPal(palPointer);
	_vm->_gfx->setPalette(palPointer);
}

void Script::sfSetPortraitBgColor(SCRIPTFUNC_PARAMS) {
	int16 red = thread->pop();
	int16 green = thread->pop();
	int16 blue = thread->pop();

	_vm->_interface->setPortraitBgColor(red, green, blue);
}

void Script::sfScriptStartCutAway(SCRIPTFUNC_PARAMS) {
	int16 cut;
	int16 fade;

	cut = thread->pop();
	thread->pop();		// Not used
	fade = thread->pop();

	_vm->_anim->setCutAwayMode(kPanelCutaway);	
	_vm->_anim->playCutaway(cut, fade != 0);
}

void Script::sfReturnFromCutAway(SCRIPTFUNC_PARAMS) {
	_vm->_anim->returnFromCutaway();
	thread->wait(kWaitTypeWakeUp);
}

void Script::sfEndCutAway(SCRIPTFUNC_PARAMS) {
	_vm->_anim->endCutaway();
}

void Script::sfGetMouseClicks(SCRIPTFUNC_PARAMS) {
	thread->_returnValue = _vm->getMouseClickCount();
}

void Script::sfResetMouseClicks(SCRIPTFUNC_PARAMS) {
	_vm->resetMouseClickCount();
}

// Used in IHNM only
// Param1: frames
void Script::sfWaitFrames(SCRIPTFUNC_PARAMS) {
	int16 frames;
	frames = thread->pop();

	if (!_skipSpeeches)
		thread->waitFrames(_vm->_frameCount + frames);
}

void Script::sfScriptFade(SCRIPTFUNC_PARAMS) {
	int16 firstPalEntry = thread->pop();
	int16 lastPalEntry = thread->pop();
	int16 startingBrightness = thread->pop();
	int16 endingBrightness = thread->pop();
	Event event;
	static PalEntry cur_pal[PAL_ENTRIES];

	_vm->_gfx->getCurrentPal(cur_pal);

	event.type = kEvTImmediate;
	event.code = kPalEvent;
	event.op = kEventPalFade;
	event.time = 0;
	event.duration = kNormalFadeDuration;
	event.data = cur_pal;
	event.param = startingBrightness;
	event.param2 = endingBrightness;
	event.param3 = firstPalEntry;
	event.param4 = lastPalEntry - firstPalEntry + 1;

	_vm->_events->queue(&event);
}

void Script::sfScriptStartVideo(SCRIPTFUNC_PARAMS) {
	int16 vid;
	int16 fade;
	vid = thread->pop();
	fade = thread->pop();

	_vm->_anim->setCutAwayMode(kPanelVideo);
	_vm->_anim->startVideo(vid, fade != 0);
}

void Script::sfScriptReturnFromVideo(SCRIPTFUNC_PARAMS) {
	_vm->_anim->returnFromVideo();
}

void Script::sfScriptEndVideo(SCRIPTFUNC_PARAMS) {
	_vm->_anim->endVideo();
}

void Script::sfShowIHNMDemoHelpBg(SCRIPTFUNC_PARAMS) {
	_ihnmDemoCurrentY = 0;
	_vm->_scene->_textList.clear();
	_vm->_interface->setMode(kPanelConverse);
	_vm->_scene->showPsychicProfile(NULL);
}

void Script::sfAddIHNMDemoHelpTextLine(SCRIPTFUNC_PARAMS) {
	int stringId, textHeight;
	TextListEntry textEntry;
	Event event;

	stringId = thread->pop();

	textHeight = _vm->_font->getHeight(kKnownFontVerb, thread->_strings->getString(stringId), 226, kFontCentered);

	textEntry.knownColor = kKnownColorBlack;
	textEntry.useRect = true;
	textEntry.rect.left = 245;
	textEntry.rect.setHeight(210 + 76);
	textEntry.rect.setWidth(226);
	textEntry.rect.top = 76 + _ihnmDemoCurrentY;
	textEntry.font = kKnownFontVerb;
	textEntry.flags = (FontEffectFlags)(kFontCentered);
	textEntry.text = thread->_strings->getString(stringId);

	TextListEntry *_psychicProfileTextEntry = _vm->_scene->_textList.addEntry(textEntry);

	event.type = kEvTOneshot;
	event.code = kTextEvent;
	event.op = kEventDisplay;
	event.data = _psychicProfileTextEntry;

	_vm->_events->queue(&event);

	_ihnmDemoCurrentY += 10;
}

void Script::sfShowIHNMDemoHelpPage(SCRIPTFUNC_PARAMS) {
	// Note: The IHNM demo changes panel mode to 8 (kPanelProtect in ITE)
	// when changing pages
	_vm->_interface->setMode(kPanelPlacard);
	_ihnmDemoCurrentY = 0;
}

void Script::sfVstopFX(SCRIPTFUNC_PARAMS) {
	_vm->_sound->stopSound();
}

void Script::sfVstopLoopedFX(SCRIPTFUNC_PARAMS) {
	_vm->_sound->stopSound();
}

void Script::sfDemoSetInteractive(SCRIPTFUNC_PARAMS) {
	int16 interactiveFlag = thread->pop();

	if (interactiveFlag == 0) {
		_vm->_interface->deactivate();
		_vm->_interface->setMode(kPanelNull);
	}

	// Note: the original also sets an appropriate flag here, but we don't,
	// as we don't use it
}

void Script::sfDemoIsInteractive(SCRIPTFUNC_PARAMS) {
	thread->_returnValue = 0;
}

void Script::sfVsetTrack(SCRIPTFUNC_PARAMS) {
	int16 chapter = thread->pop();
	int16 sceneNumber = thread->pop();
	int16 actorsEntrance = thread->pop();

	debug(2, "sfVsetTrrack(%d, %d, %d)", chapter, sceneNumber, actorsEntrance);

	_vm->_scene->changeScene(sceneNumber, actorsEntrance, kTransitionFade, chapter);
}

void Script::sfGetPoints(SCRIPTFUNC_PARAMS) {
	int16 index = thread->pop();

	if (index >= 0 && index < ARRAYSIZE(_vm->_ethicsPoints))
		thread->_returnValue = _vm->_ethicsPoints[index];
	else
		thread->_returnValue = 0;
}

void Script::sfSetGlobalFlag(SCRIPTFUNC_PARAMS) {
	int16 flag = thread->pop();

	if (flag >= 0 && flag < 32)
		_vm->_globalFlags |= (1 << flag);
}

void Script::sfClearGlobalFlag(SCRIPTFUNC_PARAMS) {
	int16 flag = thread->pop();

	if (flag >= 0 && flag < 32)
		_vm->_globalFlags &= ~(1 << flag);
}

void Script::sfTestGlobalFlag(SCRIPTFUNC_PARAMS) {
	int16 flag = thread->pop();

	if (flag >= 0 && flag < 32 && _vm->_globalFlags & (1 << flag))
		thread->_returnValue = 1;
	else
		thread->_returnValue = 0;
}

void Script::sfSetPoints(SCRIPTFUNC_PARAMS) {
	int16 index = thread->pop();
	int16 points = thread->pop();

	if (index >= 0 && index < ARRAYSIZE(_vm->_ethicsPoints))
		_vm->_ethicsPoints[index] = points;
}

void Script::sfSetSpeechBox(SCRIPTFUNC_PARAMS) {
	int16 param1 = thread->pop();
	int16 param2 = thread->pop();
	int16 param3 = thread->pop();
	int16 param4 = thread->pop();

	_vm->_actor->_speechBoxScript.left = param1;
	_vm->_actor->_speechBoxScript.top = param2;
	_vm->_actor->_speechBoxScript.setWidth(param3 - param1);
	_vm->_actor->_speechBoxScript.setHeight(param4 - param2);
}

void Script::sfDebugShowData(SCRIPTFUNC_PARAMS) {
	int16 param = thread->pop();
	char buf[50];

	snprintf(buf, 50, "Reached breakpoint %d", param);

	_vm->_interface->setStatusText(buf);
}

void Script::sfWaitFramesEsc(SCRIPTFUNC_PARAMS) {
	thread->_returnValue = _vm->_framesEsc;
}

void Script::sfQueueMusic(SCRIPTFUNC_PARAMS) {
	int16 param1 = thread->pop();
	int16 param2 = thread->pop();
	Event event;

	if (param1 < 0) {
		_vm->_music->stop();
		return;
	}

	if (param1 >= _vm->_music->_songTableLen) {
		warning("sfQueueMusic: Wrong song number (%d > %d)", param1, _vm->_music->_songTableLen - 1);
	} else {
		_vm->_music->setVolume(_vm->_musicVolume == 10 ? -1 : _vm->_musicVolume * 25, 1);
		event.type = kEvTOneshot;
		event.code = kMusicEvent;
		event.param = _vm->_music->_songTable[param1];
		event.param2 = param2 ? MUSIC_LOOP : MUSIC_NORMAL;
		event.op = kEventPlay;
		event.time = _vm->ticksToMSec(500);		// I find the delay in the original to be too long, so I've set it to
												// wait for half the time, which sounds better when chapter points
												// change
												// FIXME: If this is too short for other cases apart from chapter
												// point change, set it back to 1000

		_vm->_events->queue(&event);

		if (!_vm->_scene->haveChapterPointsChanged()) {
			_vm->_scene->setCurrentMusicTrack(param1);
			_vm->_scene->setCurrentMusicRepeat(param2);
		} else {
			// Don't save this music track when saving in IHNM
			_vm->_scene->setChapterPointsChanged(false);
		}
	}
}

void Script::sfDisableAbortSpeeches(SCRIPTFUNC_PARAMS) {
	int value = thread->pop();

	_vm->_interface->disableAbortSpeeches(value != 0);
}

void Script::sfNull(SCRIPTFUNC_PARAMS) {
	for (int i = 0; i < nArgs; i++)
		thread->pop();
}

void Script::SF_stub(const char *name, ScriptThread *thread, int nArgs) {
	char buf[256], buf1[100];

	snprintf(buf, 256, "STUB: %s(", name);

	for (int i = 0; i < nArgs; i++) {
		snprintf(buf1, 100, "%d", thread->pop());
		strncat(buf, buf1, 256);
		if (i + 1 < nArgs)
			strncat(buf, ", ", 256);
	}

	debug(0, "%s)", buf);
}

} // End of namespace Saga
