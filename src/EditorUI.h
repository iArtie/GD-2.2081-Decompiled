#pragma once

#include "cocos2d.h"
#include <string>
#include <unordered_set>
#include <unordered_map>

// Forward declarations
class LevelEditorLayer;
class GameObject;
class EffectGameObject;
class CreateMenuItem;
class EditButtonBar;
class GJRotationControl;
class GJScaleControl;
class GJTransformControl;
class ColorSelectLiveOverlay;
class HSVLiveOverlay;
class Slider;
class CCMenuItemSpriteExtra;
class CCMenuItemToggler;
class ButtonSprite;
class UndoObject;
class GJSmartTemplate;
class SetIDPopup;
class FLAlertLayer;
class GJTransformState;
class GameObjectEditorState;

class EditorUI : public cocos2d::CCLayer {
public:
    static EditorUI* create(LevelEditorLayer* editorLayer);
    
    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2);
    void activateRotationControl(cocos2d::CCObject* sender);
    void activateScaleControl(cocos2d::CCObject* sender);
    void activateTransformControl(cocos2d::CCObject* sender);
    void addObjectsToSmartTemplate(GJSmartTemplate* smartTemplate, cocos2d::CCArray* objects);
    void addSnapPosition(cocos2d::CCPoint position);
    void alignObjects(cocos2d::CCArray* objects, bool axisY);
    void anchorPointMoved(cocos2d::CCPoint anchorPoint);
    void angleChangeBegin();
    void angleChanged(float rotation);
    void applyOffset(GameObject* object);
    bool arrayContainsClass(cocos2d::CCArray* objects, int classType);
    void assignNewGroups(bool groupY);
    bool canAllowMultiActivate(GameObject* object, cocos2d::CCArray* objects);
    bool canSelectObject(GameObject* object);
    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void centerCameraOnObject(GameObject* object);
    void changeSelectedObjects(cocos2d::CCArray* objects, bool ignoreFilter);
    void checkLiveColorSelect();
    void clickOnPosition(cocos2d::CCPoint position);
    void closeLiveColorSelect();
    void closeLiveHSVSelect();
    void colorSelectClosed(cocos2d::CCNode* popup);
    void constrainGameLayerPosition();
    int convertToBaseKey(int objectID);
    std::string copyObjects(cocos2d::CCArray* objects, bool copyColors, bool sort);
    std::string copyObjectsDetailed(cocos2d::CCArray* objects);
    cocos2d::CCArray* createCustomItems();
    GameObject* createEdgeForObject(GameObject* object, int type);
    cocos2d::CCArray* createExtras(cocos2d::CCArray* objects);
    void createGlow();
    void createLoop();
    void createMoveMenu();
    void createNewKeyframeAnim();
    GameObject* createObject(int objectID, cocos2d::CCPoint position);
    cocos2d::CCArray* createOutlines(cocos2d::CCArray* objects);
    void createPrefab(GJSmartTemplate* smartTemplate, std::string key, int id);
    cocos2d::CCArray* createRockBases(cocos2d::CCArray* objects);
    cocos2d::CCArray* createRockEdges(cocos2d::CCArray* objects);
    UndoObject* createUndoObject(UndoCommand command, bool addToList);
    void createUndoSelectObject(bool redo);
    void deactivateRotationControl();
    void deactivateScaleControl();
    void deactivateTransformControl();
    void deleteObject(GameObject* object, bool noUndo);
    void deleteSmartBlocksFromObjects(cocos2d::CCArray* objects);
    void deleteTypeFromObjects(int id, cocos2d::CCArray* objects);
    void deselectAll();
    void deselectObject();
    void deselectObjectsColor();
    void deselectTargetPortals();
    void disableButton(CreateMenuItem* button);
    void doCopyObjects(bool withColor);
    void doPasteInPlace(bool withColor);
    void doPasteObjects(bool withColor);
    void draw();
    void dynamicGroupUpdate(bool reGroup);
    int edgeForObject(int id, int type);
    bool editButton2Usable();
    bool editButtonUsable();
    void editColor();
    bool editColorButtonUsable();
    void editGroup(cocos2d::CCObject* sender);
    void editHSV();
    void editObject(cocos2d::CCObject* sender);
    void editObject2(cocos2d::CCObject* sender);
    void editObject3(cocos2d::CCObject* sender);
    void editObjectSpecial(int type);
    int editorLayerForArray(cocos2d::CCArray* objects, bool layer2);
    void enableButton(CreateMenuItem* button);
    void findAndSelectObject(int id, bool useUniqueID);
    void findSnapObject(cocos2d::CCArray* objects, float offset);
    void findTriggerTest();
    void flipObjectsX(cocos2d::CCArray* objects);
    void flipObjectsY(cocos2d::CCArray* objects);
    CreateMenuItem* getCreateBtn(int id, int bg);
    GameObject* getCycledObject(cocos2d::CCArray* objects, bool dontSkipSelected);
    cocos2d::CCPoint getGridSnappedPos(cocos2d::CCPoint pos);
    cocos2d::CCPoint getGroupCenter(cocos2d::CCArray* objects, bool absolute);
    cocos2d::CCPoint getLimitedPosition(cocos2d::CCPoint position);
    CCMenuItemSpriteExtra* getModeBtn(const char* frameName, int tag);
    static int getRandomStartKey(int specialTemplate);
    cocos2d::CCPoint getRelativeOffset(GameObject* object);
    cocos2d::CCArray* getSelectedObjects();
    static int getSmartObjectKey(int key, GJSmartDirection direction);
    float getSnapAngle(GameObject* object, cocos2d::CCArray* objects);
    cocos2d::CCPoint getTouchPoint(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    cocos2d::CCNode* getTransformNode();
    GJTransformState* getTransformState();
    EditorUI* getUI();
    float getXMin(int offset);
    bool init(LevelEditorLayer* editorLayer);
    bool isLiveColorSelectTrigger(GameObject* object);
    bool isSpecialSnapObject(int id);
    void keyBackClicked();
    bool liveEditColorUsable();
    CreateMenuItem* menuItemFromObjectString(std::string str, int id);
    cocos2d::CCPoint moveForCommand(EditCommand command);
    cocos2d::CCPoint offsetForKey(int id);
    void onAssignNewGroupID();
    void onColorFilter(cocos2d::CCObject* sender);
    void onCopy(cocos2d::CCObject* sender);
    void onCopyState(cocos2d::CCObject* sender);
    bool onCreate();
    void onCreateButton(cocos2d::CCObject* sender);
    void onCreateObject(int id);
    void onDelete(cocos2d::CCObject* sender);
    void onDeleteAll(cocos2d::CCObject* sender);
    void onDeleteCustomItem(cocos2d::CCObject* sender);
    void onDeleteInfo(cocos2d::CCObject* sender);
    void onDeleteSelected(cocos2d::CCObject* sender);
    void onDeleteSelectedType(cocos2d::CCObject* sender);
    void onDeleteStartPos(cocos2d::CCObject* sender);
    void onDeselectAll(cocos2d::CCObject* sender);
    void onDuplicate(cocos2d::CCObject* sender);
    void onEditColor(cocos2d::CCObject* sender);
    void onFindObject(cocos2d::CCObject* sender);
    void onGoToBaseLayer(cocos2d::CCObject* sender);
    void onGoToLayer(cocos2d::CCObject* sender);
    void onGroupDown(cocos2d::CCObject* sender);
    void onGroupIDFilter(cocos2d::CCObject* sender);
    void onGroupSticky(cocos2d::CCObject* sender);
    void onGroupUp(cocos2d::CCObject* sender);
    void onLockLayer(cocos2d::CCObject* sender);
    void onNewCustomItem(cocos2d::CCObject* sender);
    void onPaste(cocos2d::CCObject* sender);
    void onPasteColor(cocos2d::CCObject* sender);
    void onPasteInPlace(cocos2d::CCObject* sender);
    void onPasteState(cocos2d::CCObject* sender);
    void onPause(cocos2d::CCObject* sender);
    void onPlayback(cocos2d::CCObject* sender);
    void onPlaytest(cocos2d::CCObject* sender);
    void onResetSpecialFilter(cocos2d::CCObject* sender);
    void onSelectBuildTab(cocos2d::CCObject* sender);
    void onSettings(cocos2d::CCObject* sender);
    void onStickyToggle(cocos2d::CCObject* sender);
    void onStopPlaytest(cocos2d::CCObject* sender);
    void onTargetIDChange(int change);
    void onToggleGuide(EffectGameObject* object);
    void onToggleSelectedOrder(EffectGameObject* object);
    void onToggleTraceIn(GameObject* object);
    void onToggleTraceOut(EffectGameObject* object);
    void onUngroupSticky(cocos2d::CCObject* sender);
    void onUpdateDeleteFilter(cocos2d::CCObject* sender);
    void orderDownCustomItem(cocos2d::CCObject* sender);
    void orderUpCustomItem(cocos2d::CCObject* sender);
    cocos2d::CCArray* pasteObjects(std::string str, bool withColor, bool noUndo);
    void playCircleAnim(cocos2d::CCPoint position, float radius, float duration);
    void playerTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void playerTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void playtestStopped();
    bool positionIsInSnapped(cocos2d::CCPoint position);
    cocos2d::CCPoint positionWithoutOffset(GameObject* object);
    void processSelectObjects(cocos2d::CCArray* objects);
    void recreateButtonTabs();
    void redoLastAction(cocos2d::CCObject* sender);
    void registerWithTouchDispatcher();
    void reloadCustomItems();
    void removeOffset(GameObject* object);
    void replaceGroupID(GameObject* object, int oldID, int newID);
    void resetObjectEditorValues(cocos2d::CCArray* objects);
    void resetSelectedObjectsColor();
    void resetUI();
    void rotateObjects(cocos2d::CCArray* objects, float rotation, cocos2d::CCPoint pivotPoint);
    float rotationforCommand(EditCommand command);
    void scaleChangeBegin();
    void scaleXChanged(float scaleX, bool lock);
    void scaleXYChanged(float scaleX, float scaleY, bool lock);
    void scaleYChanged(float scaleY, bool lock);
    void scrollWheel(float y, float x);
    void selectAll();
    void selectAllWithDirection(bool left);
    void selectBuildTab(int tab);
    void selectObject(GameObject* object, bool ignoreFilter);
    void selectObjects(cocos2d::CCArray* objects, bool ignoreFilter);
    void selectObjectsInRect(cocos2d::CCRect rect);
    void setIDPopupClosed(SetIDPopup* popup, int value);
    void setupCreateMenu();
    void setupDeleteMenu();
    void setupEditMenu();
    void setupTransformControl();
    bool shouldDeleteObject(GameObject* object);
    bool shouldSnap(GameObject* object);
    void showDeleteConfirmation();
    void showLiveColorSelectForMode(int colorID);
    void showMaxBasicError();
    void showMaxCoinError();
    void showMaxError();
    void showUI(bool show);
    void sliderChanged(cocos2d::CCObject* sender);
    static int smartTypeForKey(int key);
    void toggleDuplicateButton();
    void toggleEditObjectButton();
    void toggleEnableRotate(cocos2d::CCObject* sender);
    void toggleFreeMove(cocos2d::CCObject* sender);
    void toggleLockUI(bool lockUI);
    void toggleMode(cocos2d::CCObject* sender);
    void toggleObjectInfoLabel();
    void toggleSnap(cocos2d::CCObject* sender);
    void toggleSpecialEditButtons();
    void toggleSwipe(cocos2d::CCObject* sender);
    void transformChangeBegin();
    void transformObject(GameObject* object, EditCommand command, bool noOffset);
    void transformObjectsActive();
    void transformObjectsReset();
    void transformResetRotation();
    void transformRestoreRotation();
    void transformRotationChanged(float rotation);
    void transformRotationXChanged(float rotationX);
    void transformRotationYChanged(float rotationY);
    void transformScaleXChanged(float scaleX);
    void transformScaleXYChanged(float scaleX, float scaleY);
    void transformScaleYChanged(float scaleY);
    void transformSkewXChanged(float skewX);
    void transformSkewYChanged(float skewY);
    void triggerSwipeMode();
    void tryUpdateTimeMarkers();
    void undoLastAction(cocos2d::CCObject* object);
    void updateButtons();
    void updateCreateMenu(bool selectTab);
    void updateDeleteButtons();
    void updateDeleteMenu();
    void updateEditColorButton();
    void updateEditMenu();
    void updateGridNodeSize();
    void updateGroupIDBtn2();
    void updateGroupIDLabel();
    void updateObjectInfoLabel();
    void updatePlaybackBtn();
    void updateScaleControl();
    void updateSlider();
    void updateSpecialUIElements();
    void updateStickyControls();
    void updateTransformControl();
    void updateZoom(float zoom);
    float valueFromXPos(float xPos);
    float xPosFromValue(float value);
    void zoomGameLayer(bool zoomingIn);
    void zoomIn(cocos2d::CCObject* sender);
    void zoomOut(cocos2d::CCObject* sender);
    
    virtual ~EditorUI();

private:
    // Transform control members
    cocos2d::CCNode* m_transformNode;
    cocos2d::CCPoint m_pivotPoint;
    LevelEditorLayer* m_editorLayer;
    GJTransformControl* m_transformControl;
    
    // UI sprites and buttons
    ButtonSprite* m_deleteGroupSprite;
    ButtonSprite* m_deleteColorSprite;
    
    // Selection mode
    int m_selectedMode;
    
    // Swipe members
    bool m_swipeModeTriggered;
    bool m_swipeActive;
    cocos2d::CCPoint m_swipeEnd;
    cocos2d::CCPoint m_swipeStart;
    bool m_continuousSnap;
    GameObject* m_snapObject;
    bool m_snapObjectExists;
    
    // Delegates
    void* m_setIDPopupDelegate;  // SetIDPopupDelegate
    
    // UI toggle states
    bool m_swipeEnabled;
    CCMenuItemSpriteExtra* m_swipeBtn;
    bool m_freeMoveEnabled;
    CCMenuItemSpriteExtra* m_freeMoveBtn;
    CCMenuItemSpriteExtra* m_snapBtn;
    
    // Selection
    GameObject* m_selectedObject;
    cocos2d::CCArray* m_selectedObjects;
    
    // Sticky controls
    bool m_isPaused;
    bool m_stickyControlsEnabled;
    bool m_linkControlsDisabled;
    CCMenuItemSpriteExtra* m_enableLinkBtn;
    CCMenuItemSpriteExtra* m_linkBtn;
    CCMenuItemSpriteExtra* m_unlinkBtn;
    
    int m_selectedObjectIndex;
    bool m_reloadItems;
    cocos2d::CCArray* m_createButtonBars;
    EditButtonBar* m_editButtonBar;
    cocos2d::CCMenu* m_deleteMenu;
    float m_toolbarHeight;
    CCMenuItemSpriteExtra* m_deleteButton;
    cocos2d::CCSprite* m_deleteAllSprite;
    CCMenuItemSpriteExtra* m_deleteAllButton;
    CCMenuItemSpriteExtra* m_deleteFilterNone;
    void* m_deleteFilterStatic;
    CCMenuItemSpriteExtra* m_deleteFilterDetails;
    cocos2d::CCSprite* m_customDeleteSprite;
    CCMenuItemSpriteExtra* m_deleteFilterCustom;
    
    // Snap
    cocos2d::CCArray* m_snapPositions;
    
    // Playback
    CCMenuItemSpriteExtra* m_playbackBtn;
    bool m_isPlayingMusic;
    bool m_playbackActive;
    float m_playbackStartWarp;
    float m_playbackStartTime;
    bool m_alertShown;
    CCMenuItemSpriteExtra* m_goToBaseBtn;
    
    // Create menu
    int m_selectedTab;
    EditButtonBar* m_createButtonBar;
    cocos2d::CCArray* m_tabsArray;
    cocos2d::CCMenu* m_tabsMenu;
    cocos2d::CCArray* m_createButtonArray;
    cocos2d::CCArray* m_customObjectButtonArray;
    bool m_speedObjectsUpdated;
    
    // Mode buttons
    CCMenuItemSpriteExtra* m_deleteModeBtn;
    CCMenuItemSpriteExtra* m_buildModeBtn;
    CCMenuItemSpriteExtra* m_editModeBtn;
    CCMenuItemSpriteExtra* m_copyPasteBtn;
    CCMenuItemSpriteExtra* m_copyBtn;
    CCMenuItemSpriteExtra* m_pasteBtn;
    
    // Edit controls
    cocos2d::CCDictionary* m_editButtonDict;
    CCMenuItemSpriteExtra* m_editHSVBtn;
    ColorSelectLiveOverlay* m_colorOverlay;
    CCMenuItemSpriteExtra* m_editObjectBtn;
    CCMenuItemSpriteExtra* m_editSpecialBtn;
    bool m_canActivateControls;
    HSVLiveOverlay* m_hsvOverlay;
    void* m_colorSelectDelegate;  // ColorSelectDelegate
    
    // Layer controls
    cocos2d::CCLabelBMFont* m_currentLayerLabel;
    cocos2d::CCSprite* m_layerLockSprite;
    cocos2d::CCArray* m_uiItems;
    float m_gridSize;
    float m_constrainedHeight;
    Slider* m_positionSlider;
    
    // Touch
    int m_playerTouchID2;
    int m_playerTouchID1;
    int m_cycledObjectIndex;
    bool m_refreshPosition;
    GJRotationControl* m_rotationControl;
    int m_rotationTouchID;
    GJScaleControl* m_scaleControl;
    CCMenuItemSpriteExtra* m_playtestBtn;
    CCMenuItemSpriteExtra* m_playtestStopBtn;
    float m_editorZoom;
    
    // Custom objects
    cocos2d::CCArray* m_customTabControls;
    EditButtonBar* m_customTabBar;
    
    // Additional UI buttons
    CCMenuItemSpriteExtra* m_deselectBtn;
    CCMenuItemSpriteExtra* m_undoBtn;
    CCMenuItemSpriteExtra* m_trashBtn;
    CCMenuItemSpriteExtra* m_redoBtn;
    CCMenuItemSpriteExtra* m_editGroupBtn;
    CCMenuItemSpriteExtra* m_copyValuesBtn;
    CCMenuItemSpriteExtra* m_goToLayerBtn;
    CCMenuItemSpriteExtra* m_pasteStateBtn;
    CCMenuItemSpriteExtra* m_pasteColorBtn;
    void* m_rotateBtn;
    
    // Transform state
    GJTransformState* m_transformState;
    std::unordered_map<int, GameObjectEditorState> m_objectEditorStates;
    
    // UI scaling
    bool m_increaseScale;
    cocos2d::CCLabelBMFont* m_objectInfoLabel;
    
    // Layer navigation
    CCMenuItemSpriteExtra* m_layerNextBtn;
    CCMenuItemSpriteExtra* m_layerPrevBtn;
    
    // Touch IDs
    int m_scaleTouchID;
    int m_touchID;
    int m_transformTouchID;
    
    // Editor state
    cocos2d::CCArray* m_deleteObjects;
    bool m_snapSelected;
    bool m_findSnap;
    cocos2d::CCPoint m_clickAtPosition;
    cocos2d::CCPoint m_createPosition;
    bool m_continueSwipe;
    cocos2d::CCPoint m_cameraTest;
    bool m_spaceSwiping;
    bool m_isDraggingCamera;
    cocos2d::CCPoint m_lastTouchPoint;
    cocos2d::CCPoint m_snapPosition;
    bool m_noSnapUndo;
    UndoObject* m_undoObject;
    bool m_swipeSelected;
    bool m_editingObject;
    cocos2d::CCPoint m_swipePosition;
    double m_keyTime;
    double m_keyTime2;
};

#endif