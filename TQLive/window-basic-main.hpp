/******************************************************************************
    Copyright (C) 2013-2014 by Hugh Bailey <obs.jim@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#pragma once

#include <QBuffer>
#include <QAction>
#include <QWidgetAction>
#include <QSystemTrayIcon>
#include <obs.hpp>
#include <vector>
#include <memory>
#include "window-main.hpp"
#include "window-basic-interaction.hpp"
#include "window-basic-properties.hpp"
#include "window-basic-transform.hpp"
#include "window-basic-adv-audio.hpp"
#include "window-basic-filters.hpp"
#include "window-projector.hpp"
#include "window-basic-about.hpp"
#include "auth-base.hpp"

#include <obs-frontend-internal.hpp>

#include <util/platform.h>
#include <util/threading.h>
#include <util/util.hpp>

#include <QPointer>
#include "CCustomMainWindow.h"
#include "EnumData.h"
#include "HttpAgent.h"

class QMessageBox;
class QListWidgetItem;
class VolControl;
class OBSBasicStats;
class BarrageAssistantDlg;
class SystemSettingsDlg;
class TQLiveAboutDlg;
class RoomManageAssistantDlg;
class GroupChatSettingsDlg;
class RedEnvelopeDlg;
class ExitAppTipsDlg;
class QSystemTrayIcon;
class BroadcastReminderDlg;
class OffcastStatisticsDlg;
class ScreenCaptureSelectedDlg;
class CustomArrowWgt;
class AudioControlWindow;
class AddMediaSourceDlg;
class AddCameraSourceDlg;
class AddImageSourceDlg;
class AddTextSourceDlg;
class CWaterMark;

#include "ui_OBSBasic.h"
#include "ui_ColorSelect.h"
#include "CDataCentre.h"
#include "CWaterMark.h"

#define DESKTOP_AUDIO_1 Str("DesktopAudioDevice1")
#define DESKTOP_AUDIO_2 Str("DesktopAudioDevice2")
#define AUX_AUDIO_1     Str("AuxAudioDevice1")
#define AUX_AUDIO_2     Str("AuxAudioDevice2")
#define AUX_AUDIO_3     Str("AuxAudioDevice3")
#define AUX_AUDIO_4     Str("AuxAudioDevice4")

#define SIMPLE_ENCODER_X264                    "x264"
#define SIMPLE_ENCODER_X264_LOWCPU             "x264_lowcpu"
#define SIMPLE_ENCODER_QSV                     "qsv"
#define SIMPLE_ENCODER_NVENC                   "nvenc"
#define SIMPLE_ENCODER_AMD                     "amd"

//#define PREVIEW_EDGE_SIZE 10
#define PREVIEW_EDGE_SIZE 0
#define LABEL_MAX_LENGTH 8

struct BasicOutputHandler;

enum class QtDataRole {
	OBSRef = Qt::UserRole,
	OBSSignals,
};

struct SavedProjectorInfo {
	ProjectorType type;
	int monitor;
	std::string geometry;
	std::string name;
};

struct QuickTransition {
	QPushButton *button = nullptr;
	OBSSource source;
	obs_hotkey_id hotkey = OBS_INVALID_HOTKEY_ID;
	int duration = 0;
	int id = 0;

	inline QuickTransition() {}
	inline QuickTransition(OBSSource source_, int duration_, int id_)
		: source        (source_),
		  duration      (duration_),
		  id            (id_),
		  renamedSignal (std::make_shared<OBSSignal>(
					obs_source_get_signal_handler(source),
					"rename", SourceRenamed, this))
	{}

private:
	static void SourceRenamed(void *param, calldata_t *data);
	std::shared_ptr<OBSSignal> renamedSignal;
};

class ColorSelect : public QWidget {

public:
	explicit ColorSelect(QWidget *parent = 0);

private:
	std::unique_ptr<Ui::ColorSelect> ui;
};

#define ZONE_MEASURE 5

enum EN_ZONE_TYPE 
{ 
	CENTRE,
	TOP, 
	BOTTOM,
	LEFTTOP,
	LEFT,
	LEFTBOTTOM,
	RIGHTTOP,
	RIGHT, 
	RIGHTBOTTOM, 
};

class OBSBasic : public OBSMainWindow {
	Q_OBJECT

	friend class OBSBasicPreview;
	friend class OBSBasicStatusBar;
	friend class OBSBasicSourceSelect;
	friend class OBSBasicSettings;
	friend class Auth;
	friend class AutoConfig;
	friend class AutoConfigStreamPage;
	friend struct OBSStudioAPI;

	enum class MoveDir {
		Up,
		Down,
		Left,
		Right
	};

	enum DropType {
		DropType_RawText,
		DropType_Text,
		DropType_Image,
		DropType_Media,
		DropType_Html
	};

	struct ST_LIVE_CONTENT_INFO
	{
		bool bIsScheduleData;
		int nSelectionCategoryID;
		int nSelectionMatchID;
		int nScheduleRecordID;
		int nGroupID;
		
		ST_LIVE_CONTENT_INFO()
			: bIsScheduleData(false)
			, nGroupID(-1)
			, nSelectionCategoryID(-1)
			, nSelectionMatchID(-1)
			, nScheduleRecordID(-1)
		{
		}
		
	};

private:
	BarrageAssistantDlg *m_pBarrageAssistantDlg;
	SystemSettingsDlg *m_pSystemSettingsDlg;
	TQLiveAboutDlg *m_pTQLiveAboutDlg;
	RoomManageAssistantDlg *m_pRoomManageAssistantDlg;
	GroupChatSettingsDlg *m_pGroupChatSettingsDlg;
	RedEnvelopeDlg *m_pRedEnvelopeDlg;
	ExitAppTipsDlg *m_pExitAppTipsDlg;
	QSystemTrayIcon *m_pSystemTray;
	BroadcastReminderDlg *m_pBroadcastReminderDlg;
	OffcastStatisticsDlg *m_pOffcastStatisticsDlg;
	CustomArrowWgt *m_pSoundEffectMenu;
	AudioControlWindow *m_pAudioControlWin;

	int m_nLiveAbortTpye;
	int m_nBarrageMsgTotal;  //弹幕统计个数
	int m_nGeneralUserTotal; //普通用户人数
	int m_nVipUserTotal;     //Vip用户人数
	int m_nRealBarrageMsgTotal; // 真实弹幕个数
	bool m_pIsNeedToUpdate = false;
	QList<AddMediaSourceDlg*> m_pMediaDlgList;
	QList<AddCameraSourceDlg*> m_pCameraDlgList;
	QList<AddImageSourceDlg*> m_pImageDlgList;
	QList<AddTextSourceDlg*> m_pTextDlgList;

	QRect m_rcWndMeasure;

	obs_frontend_callbacks *api = nullptr;

	std::shared_ptr<Auth> auth;

	std::vector<VolControl*> volumes;

	std::vector<OBSSignal> signalHandlers;

	QList<QPointer<QDockWidget>> extraDocks;

	bool loaded = false;
	long disableSaving = 1;
	bool projectChanged = false;
	bool previewEnabled = true;
	bool fullscreenInterface = false;

	const char *copyString;
	const char *copyFiltersString = nullptr;
	bool copyVisible = true;

	ST_LIVE_CONTENT_INFO m_stLiveContentInfo;


	QTimer *m_timerHeartbeatValidation;

	QScopedPointer<QThread> updateCheckThread;
	QScopedPointer<QThread> introCheckThread;
	QScopedPointer<QThread> logUploadThread;

	QPointer<OBSBasicInteraction> interaction;
	QPointer<OBSBasicProperties> properties;
	QPointer<OBSBasicTransform> transformWindow;
	QPointer<OBSBasicAdvAudio> advAudioWindow;
	QPointer<OBSBasicFilters> filters;
	QPointer<QDockWidget> statsDock;
	QPointer<OBSAbout> about;

	QPointer<QTimer>    cpuUsageTimer;
	os_cpu_usage_info_t *cpuUsageInfo = nullptr;

	OBSService service;
	std::unique_ptr<BasicOutputHandler> outputHandler;
	bool streamingStopping = false;
	bool recordingStopping = false;
	bool replayBufferStopping = false;

	gs_vertbuffer_t *box = nullptr;
	gs_vertbuffer_t *boxLeft = nullptr;
	gs_vertbuffer_t *boxTop = nullptr;
	gs_vertbuffer_t *boxRight = nullptr;
	gs_vertbuffer_t *boxBottom = nullptr;
	gs_vertbuffer_t *circle = nullptr;

	bool          sceneChanging = false;
	bool          ignoreSelectionUpdate = false;

	int           previewX = 0,  previewY = 0;
	int           previewCX = 0, previewCY = 0;
	float         previewScale = 0.0f;

	ConfigFile    basicConfig;

	std::vector<SavedProjectorInfo*> savedProjectorsArray;
	QPointer<QWidget> projectors[10];
	QList<QPointer<QWidget>> windowProjectors;

	QPointer<QWidget> stats;
	QPointer<QWidget> remux;

	QPointer<QMenu> startStreamMenu;

	QPointer<QPushButton> transitionButton;
	QPointer<QPushButton> replayBufferButton;

	QScopedPointer<QSystemTrayIcon> trayIcon;
	QPointer<QAction>         sysTrayStream;
	QPointer<QAction>         sysTrayRecord;
	QPointer<QAction>         sysTrayReplayBuffer;
	QPointer<QAction>         showHide;
	QPointer<QAction>         exit;
	QPointer<QMenu>           trayMenu;
	QPointer<QMenu>           previewProjector;
	QPointer<QMenu>           studioProgramProjector;
	QPointer<QMenu>           multiviewProjectorMenu;
	QPointer<QMenu>           previewProjectorSource;
	QPointer<QMenu>           previewProjectorMain;
	QPointer<QMenu>           sceneProjectorMenu;
	QPointer<QMenu>           sourceProjector;
	QPointer<QMenu>           scaleFilteringMenu;
	QPointer<QMenu>           colorMenu;
	QPointer<QWidgetAction>   colorWidgetAction;
	QPointer<ColorSelect>     colorSelect;
	QPointer<QMenu>           deinterlaceMenu;
	QPointer<QMenu>           perSceneTransitionMenu;
	QPointer<QObject>         shortcutFilter;

	QPointer<QWidget> programWidget;
	QPointer<QVBoxLayout> programLayout;
	QPointer<QLabel> programLabel;

	QRect m_szDropArea[9];

	void          UpdateMultiviewProjectorMenu();

	void          DrawBackdrop(float cx, float cy);

	void          SetupEncoders();

	void          CreateFirstRunSources();
	void          CreateDefaultScene(bool firstStart);

	void          UpdateVolumeControlsDecayRate();
	void          UpdateVolumeControlsPeakMeterType();
	void          ClearVolumeControls();

	void          UploadLog(const char *subdir, const char *file);

	void          Save(const char *file);
	void          Load(const char *file);

	void          InitHotkeys();
	void          CreateHotkeys();
	void          ClearHotkeys();

	bool          InitService();

	bool          InitBasicConfigDefaults();
	void          InitBasicConfigDefaults2();
	bool          InitBasicConfig();

	void          InitOBSCallbacks();
	void          InitSystemTray();

	void          InitPrimitives();

	void 	      OnFirstLoad();

	OBSSceneItem  GetSceneItem(QListWidgetItem *item);
	OBSSceneItem  GetCurrentSceneItem();

	bool          QueryRemoveSource(obs_source_t *source);

	void          TimedCheckForUpdates();
	void          CheckForUpdates(bool manualUpdate);

	void GetFPSCommon(uint32_t &num, uint32_t &den) const;
	void GetFPSInteger(uint32_t &num, uint32_t &den) const;
	void GetFPSFraction(uint32_t &num, uint32_t &den) const;
	void GetFPSNanoseconds(uint32_t &num, uint32_t &den) const;
	void GetConfigFPS(uint32_t &num, uint32_t &den) const;

	void UpdatePreviewScalingMenu();

	void LoadSceneListOrder(obs_data_array_t *array);
	obs_data_array_t *SaveSceneListOrder();
	void ChangeSceneIndex(bool relative, int idx, int invalidIdx);

	void TempFileOutput(const char *path, int vBitrate, int aBitrate);
	void TempStreamOutput(const char *url, const char *key,
			int vBitrate, int aBitrate);

	void CloseDialogs();
	void ClearSceneData();

	void Nudge(int dist, MoveDir dir);

	OBSProjector *OpenProjector(obs_source_t *source, int monitor,
			QString title, ProjectorType type);

	void GetAudioSourceFilters();
	void GetAudioSourceProperties();
	void VolControlContextMenu();
	void ToggleVolControlLayout();
	void ToggleMixerLayout(bool vertical);

	void RefreshSceneCollections();
	void ChangeSceneCollection();
	void LogScenes();

	void LoadProfile();
	void ResetProfileData();
	bool AddProfile(bool create_new, const char *title, const char *text,
			const char *init_text = nullptr, bool rename = false);
	void DeleteProfile(const char *profile_name, const char *profile_dir);
	void RefreshProfiles();
	void ChangeProfile();
	void CheckForSimpleModeX264Fallback();

	void SaveProjectNow();

	int GetTopSelectedSourceItem();

	obs_hotkey_pair_id streamingHotkeys, recordingHotkeys,
	                   replayBufHotkeys;
	obs_hotkey_id forceStreamingStopHotkey;

	void InitDefaultTransitions();
	void InitTransition(obs_source_t *transition);
	obs_source_t *FindTransition(const char *name);
	OBSSource GetCurrentTransition();
	obs_data_array_t *SaveTransitions();
	void LoadTransitions(obs_data_array_t *transitions);

	obs_source_t *fadeTransition;

	void CreateProgramDisplay();
	void CreateProgramOptions();
	void AddQuickTransitionId(int id);
	void AddQuickTransition();
	void AddQuickTransitionHotkey(QuickTransition *qt);
	void RemoveQuickTransitionHotkey(QuickTransition *qt);
	void LoadQuickTransitions(obs_data_array_t *array);
	obs_data_array_t *SaveQuickTransitions();
	void ClearQuickTransitionWidgets();
	void RefreshQuickTransitions();
	void DisableQuickTransitionWidgets();
	void EnableQuickTransitionWidgets();
	void CreateDefaultQuickTransitions();

	QMenu *CreatePerSceneTransitionMenu();

	QuickTransition *GetQuickTransition(int id);
	int GetQuickTransitionIdx(int id);
	QMenu *CreateTransitionMenu(QWidget *parent, QuickTransition *qt);
	void ClearQuickTransitions();
	void QuickTransitionClicked();
	void QuickTransitionChange();
	void QuickTransitionChangeDuration(int value);
	void QuickTransitionRemoveClicked();

	void SetPreviewProgramMode(bool enabled);
	void ResizeProgram(uint32_t cx, uint32_t cy);
	void SetCurrentScene(obs_scene_t *scene, bool force = false,
			bool direct = false);
	static void RenderProgram(void *data, uint32_t cx, uint32_t cy);

	std::vector<QuickTransition> quickTransitions;
	QPointer<QWidget> programOptions;
	QPointer<OBSQTDisplay> program;
	OBSWeakSource lastScene;
	OBSWeakSource swapScene;
	OBSWeakSource programScene;
	bool editPropertiesMode = false;
	bool sceneDuplicationMode = true;
	bool swapScenesMode = true;
	volatile bool previewProgramMode = false;
	obs_hotkey_id togglePreviewProgramHotkey = 0;
	obs_hotkey_id transitionHotkey = 0;
	int quickTransitionIdCounter = 1;
	bool overridingTransition = false;

	int   programX = 0,  programY = 0;
	int   programCX = 0, programCY = 0;
	float programScale = 0.0f;

	int disableOutputsRef = 0;

	inline void OnActivate();
	inline void OnDeactivate();

	void AddDropSource(const char *file, DropType image);
	void dragEnterEvent(QDragEnterEvent *event) override;
	void dragLeaveEvent(QDragLeaveEvent *event) override;
	void dragMoveEvent(QDragMoveEvent *event) override;
	void dropEvent(QDropEvent *event) override;

	void ReplayBufferClicked();

	bool sysTrayMinimizeToTray();

	void EnumDialogs();

	QList<QDialog*> visDialogs;
	QList<QDialog*> modalDialogs;
	QList<QMessageBox*> visMsgBoxes;

	QList<QPoint> visDlgPositions;

	QByteArray startingDockLayout;

	obs_data_array_t *SaveProjectors();
	void LoadSavedProjectors(obs_data_array_t *savedProjectors);

	void ReceivedIntroJson(const QString &text);

	bool NoSourcesConfirmation();

	bool m_isMLBPressed;								// 缩放区域判断左键是否按下
	QPoint dragPosition;								// 窗口移动拖动时需要记住的点 
	EN_ZONE_TYPE m_eZoneType;							// 窗口大小改变时，记录改变方向

protected:
	virtual void leaveEvent(QEvent *event);
	//virtual void enterEvent(QEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	//void paintEvent(QPaintEvent *event);

	//bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;
	//void mousePressEvent(QMouseEvent* e) override;
	//void mouseMoveEvent(QMouseEvent* e) override;

	//int m_bouderWidth;
	//QPoint m_curPos;

public:
	void ProhibitLiveProcess();
	void ValidateHeartbeatFailureProcess();

public slots:
	void DeferSaveBegin();
	void DeferSaveEnd();

	void StartStreaming();
	void StopStreaming();
	void ForceStopStreaming();

	void StreamDelayStarting(int sec);
	void StreamDelayStopping(int sec);

	void StreamingStart();
	void StreamStopping();
	void StreamingStop(int errorcode, QString last_error);

	void StartRecording();
	void StopRecording();

	void RecordingStart();
	void RecordStopping();
	void RecordingStop(int code);

	void StartReplayBuffer();
	void StopReplayBuffer();

	void ReplayBufferStart();
	void ReplayBufferSave();
	void ReplayBufferStopping();
	void ReplayBufferStop(int code);

	void SaveProjectDeferred();
	void SaveProject();

	void SetTransition(OBSSource transition);
	void TransitionToScene(OBSScene scene, bool force = false,
			bool direct = false);
	void TransitionToScene(OBSSource scene, bool force = false,
			bool direct = false, bool quickTransition = false);
	void SetCurrentScene(OBSSource scene, bool force = false,
			bool direct = false);

	bool AddSceneCollection(
			bool create_new,
			const QString &name = QString());

	void slotShowMaximizedOrNormal();
	void slotModifyLiveRoomName();
	void slotModificationSave();
	void slotModificationCancel();
	//void slotUpdateLiveContent(const QString &qsCategoryName, const QString &qsContentName);
	void slotUpdateLiveContent(ST_MATCH_INFO &stMatchInfo);
	void slotModifyLiveContent();
	void slotAcquireLivePushAddress(int, const QString&);
	void slotUpdateLiveContentTask(int, QString, const void *);
	void slotFinishLive(int, QString);
	void slotLogout(int nStateCode, QString qsMsg);
	void slotSubmitHeartbeatValidation();
	void slotHeartbeatValidation(int nStateCode, QString qsMsg);
	//void slotIMAcquireGroupInfo(int nStatuCode, const QString &qsMsg, int nGroupNum);
	void slotIMAcquireGroupInfo(int nStatuCode, const QString &qsMsg, const ST_GROUP_INFO &stGroupInfo);
	void slotTurnoffDlg(int nUserData);
	//void slotHeartbeatValidationFailure();
	void slotAcquireDefaultLiveContent(int nStatuCode, const QString &qsMsg, const ST_LIVE_CONTENT &stLiveContent);

	void slotAcquireOSSInformation(int nStatusCode, const QString &qsMsg, const ST_OSS_INFORMATION &stOSSInformation, const void *userData);
	void slotUploadFile(int nStatusCode, const QString &qsMsg, const QString &qsFileID, const void *userData);


	void slotAcquireLiveCategory(int nStatusCode, const QString &qsMsg, const QList<ST_LIVE_CATEGORY_INFO> &obList, const void *userData);
	void onCheckForUpdates(int nStatusCode, const QString &qsMsg, const ST_VER_INFO &stVerInfo, const void *userData);
	void onCatchException(int errCode, QString errMsg);
	void onGetChatGroupInfo(int nStatusCode, const QString &qsMsg, const ST_GROUP_INFO &stGroupInfo);
	void onIMLoginRespond();
	void onWarningTips(int nStatusCode, QString sMsg, int nLimitTime);
	void onUpdateBarrageMsgTotal(int barrageMsgTotal, int generalUserTotal, int vipUserTotal, int realBarrageMsgTotal);
	void onHaveSourceItems(bool bHave);
	void onShowSoundEffectMenu();
	void onJblValueChanged(int val);
	void onMicValueChanged(int val);
	void onCreateSourceItem(OBSSceneItem item);

private slots:
	void AddSceneItem(OBSSceneItem item);
	void AddScene(OBSSource source);
	void RemoveScene(OBSSource source);
	void RenameSources(OBSSource source, QString newName, QString prevName);

	void SelectSceneItem(OBSScene scene, OBSSceneItem item, bool select);

	void ActivateAudioSource(OBSSource source);
	void DeactivateAudioSource(OBSSource source);

	void DuplicateSelectedScene();
	void RemoveSelectedScene();
	void RemoveSelectedSceneItem();

	void ToggleAlwaysOnTop();

	void ReorderSources(OBSScene scene);

	void ProcessHotkey(obs_hotkey_id id, bool pressed);

	void AddTransition();
	void RenameTransition();
	void TransitionClicked();
	void TransitionStopped();
	void TransitionFullyStopped();
	void TriggerQuickTransition(int id);

	void SetDeinterlacingMode();
	void SetDeinterlacingOrder();

	void SetScaleFilter();

	void IconActivated(QSystemTrayIcon::ActivationReason reason);
	void SetShowing(bool showing);

	void ToggleShowHide();

	void HideAudioControl();
	void UnhideAllAudioControls();
	void ToggleHideMixer();

	void MixerRenameSource();

	void on_vMixerScrollArea_customContextMenuRequested();
	void on_hMixerScrollArea_customContextMenuRequested();

	void on_actionCopySource_triggered();
	void on_actionPasteRef_triggered();
	void on_actionPasteDup_triggered();

	void on_actionCopyFilters_triggered();
	void on_actionPasteFilters_triggered();

	void ColorChange();

	SourceTreeItem *GetItemWidgetFromSceneItem(obs_sceneitem_t *sceneItem);

	void on_actionShowAbout_triggered();

	void AudioMixerCopyFilters();
	void AudioMixerPasteFilters();

private:
	/* OBS Callbacks */
	static void SceneReordered(void *data, calldata_t *params);
	static void SceneItemAdded(void *data, calldata_t *params);
	static void SceneItemSelected(void *data, calldata_t *params);
	static void SceneItemDeselected(void *data, calldata_t *params);
	static void SourceCreated(void *data, calldata_t *params);
	static void SourceRemoved(void *data, calldata_t *params);
	static void SourceActivated(void *data, calldata_t *params);
	static void SourceDeactivated(void *data, calldata_t *params);
	static void SourceRenamed(void *data, calldata_t *params);
	static void RenderMain(void *data, uint32_t cx, uint32_t cy);

	void ResizePreview(uint32_t cx, uint32_t cy);

	void AddSource(const char *id);
	QMenu *CreateAddSourcePopupMenu();
	void AddSourcePopupMenu(const QPoint &pos);
	void copyActionsDynamicProperties();

	static void HotkeyTriggered(void *data, obs_hotkey_id id, bool pressed);

	void AutoRemux();

	void checkForUpdates(const ST_VER_INFO &verInfo, const QString &currVer);

public:
	OBSSource GetProgramSource();
	OBSScene GetCurrentScene();
	void InitializeCusorDropArea();
	void SetCursorShape(const QPoint &ptCursorPos);

	void SysTrayNotify(const QString &text, QSystemTrayIcon::MessageIcon n);

	inline OBSSource GetCurrentSceneSource()
	{
		OBSScene curScene = GetCurrentScene();
		return OBSSource(obs_scene_get_source(curScene));
	}

	obs_service_t *GetService();
	void          SetService(obs_service_t *service);

	inline bool IsPreviewProgramMode() const
	{
		return os_atomic_load_bool(&previewProgramMode);
	}

	bool StreamingActive() const;
	bool Active() const;

	void ResetUI();
	int  ResetVideo();
	bool ResetAudio();

	void ResetOutputs();

	void ResetAudioDevice(const char *sourceId, const char *deviceId,
			const char *deviceDesc, int channel);

	void NewProject();
	void LoadProject();

	inline void GetDisplayRect(int &x, int &y, int &cx, int &cy)
	{
		x  = previewX;
		y  = previewY;
		cx = previewCX;
		cy = previewCY;
	}

	inline bool SavingDisabled() const
	{
		return disableSaving;
	}

	inline double GetCPUUsage() const
	{
		return os_cpu_usage_info_query(cpuUsageInfo);
	}

	void SaveService();
	bool LoadService();

	inline Auth *GetAuth() {return auth.get();}

	inline void EnableOutputs(bool enable)
	{
		if (enable) {
			if (--disableOutputsRef < 0)
				disableOutputsRef = 0;
		} else {
			disableOutputsRef++;
		}
	}

	QMenu *AddDeinterlacingMenu(QMenu *menu, obs_source_t *source);
	QMenu *AddScaleFilteringMenu(QMenu *menu, obs_sceneitem_t *item);
	QMenu *AddBackgroundColorMenu(QMenu *menu, QWidgetAction *widgetAction,
			ColorSelect *select, obs_sceneitem_t *item);
	void CreateSourcePopupMenu(int idx, bool preview);

	void UpdateTitleBar();
	void UpdateSceneSelection(OBSSource source);

	void SystemTrayInit();
	void SystemTray(bool firstStarted);

	void OpenSavedProjectors();

	void CreateInteractionWindow(obs_source_t *source);
	void CreatePropertiesWindow(obs_source_t *source);
	void CreateFiltersWindow(obs_source_t *source);

	QAction *AddDockWidget(QDockWidget *dock);

	static OBSBasic *Get();

	void ResetCurrentService();
	void CancelCurrentSource(QString sourceName);

protected:
	virtual void closeEvent(QCloseEvent *event) override;
	virtual void changeEvent(QEvent *event) override;
	void showEvent(QShowEvent *ev);
	void resizeEvent(QResizeEvent *event);
	virtual bool eventFilter(QObject *watched, QEvent *event);

public slots:
	void on_actionFullscreenInterface_triggered();

	void on_actionShow_Recordings_triggered();
	void on_actionRemux_triggered();
	void on_action_Settings_triggered();
	void on_actionAdvAudioProperties_triggered();
	void on_advAudioProps_clicked();
	void on_advAudioProps_destroyed();
	void on_actionShowLogs_triggered();
	void on_actionUploadCurrentLog_triggered();
	void on_actionUploadLastLog_triggered();
	void on_actionViewCurrentLog_triggered();
	void on_actionCheckForUpdates_triggered();

	void on_actionShowCrashLogs_triggered();
	void on_actionUploadLastCrashLog_triggered();

	void on_actionEditTransform_triggered();
	void on_actionCopyTransform_triggered();
	void on_actionPasteTransform_triggered();
	void on_actionRotate90CW_triggered();
	void on_actionRotate90CCW_triggered();
	void on_actionRotate180_triggered();
	void on_actionFlipHorizontal_triggered();
	void on_actionFlipVertical_triggered();
	void on_actionFitToScreen_triggered();
	void on_actionStretchToScreen_triggered();
	void on_actionCenterToScreen_triggered();

	void on_scenes_currentItemChanged(QListWidgetItem *current,
			QListWidgetItem *prev);
	void on_scenes_customContextMenuRequested(const QPoint &pos);
	void on_actionAddScene_triggered();
	void on_actionRemoveScene_triggered();
	void on_actionSceneUp_triggered();
	void on_actionSceneDown_triggered();
	void on_sources_customContextMenuRequested(const QPoint &pos);
	void on_scenes_itemDoubleClicked(QListWidgetItem *item);
	void on_actionAddSource_triggered();
	void on_actionRemoveSource_triggered();
	void on_actionInteract_triggered();
	void on_actionSourceProperties_triggered();
	void on_actionSourceUp_triggered();
	void on_actionSourceDown_triggered();

	void on_actionMoveUp_triggered();
	void on_actionMoveDown_triggered();
	void on_actionMoveToTop_triggered();
	void on_actionMoveToBottom_triggered();

	void on_actionLockPreview_triggered();

	void on_scalingMenu_aboutToShow();
	void on_actionScaleWindow_triggered();
	void on_actionScaleCanvas_triggered();
	void on_actionScaleOutput_triggered();

	void on_streamButton_clicked();
	void on_recordButton_clicked();
	void on_settingsButton_clicked();

	void on_actionHelpPortal_triggered();
	void on_actionWebsite_triggered();
	void on_actionDiscord_triggered();

	void on_preview_customContextMenuRequested(const QPoint &pos);
	void on_program_customContextMenuRequested(const QPoint &pos);
	void on_previewDisabledLabel_customContextMenuRequested(
			const QPoint &pos);

	void on_actionNewSceneCollection_triggered();
	void on_actionDupSceneCollection_triggered();
	void on_actionRenameSceneCollection_triggered();
	void on_actionRemoveSceneCollection_triggered();
	void on_actionImportSceneCollection_triggered();
	void on_actionExportSceneCollection_triggered();

	void on_actionNewProfile_triggered();
	void on_actionDupProfile_triggered();
	void on_actionRenameProfile_triggered();
	void on_actionRemoveProfile_triggered();
	void on_actionImportProfile_triggered();
	void on_actionExportProfile_triggered();

	void on_actionShowSettingsFolder_triggered();
	void on_actionShowProfileFolder_triggered();

	void on_actionAlwaysOnTop_triggered();

	void on_toggleListboxToolbars_toggled(bool visible);
	void on_toggleStatusBar_toggled(bool visible);

	void on_transitions_currentIndexChanged(int index);
	void on_transitionAdd_clicked();
	void on_transitionRemove_clicked();
	void on_transitionProps_clicked();

	void on_modeSwitch_clicked();

	void on_autoConfigure_triggered();
	void on_stats_triggered();

	void on_resetUI_triggered();
	void on_lockUI_toggled(bool lock);

	void logUploadFinished(const QString &text, const QString &error);

	void updateCheckFinished();

	void AddSourceFromAction();

	void MoveSceneToTop();
	void MoveSceneToBottom();

	void EditSceneName();
	void EditSceneItemName();

	void SceneNameEdited(QWidget *editor,
			QAbstractItemDelegate::EndEditHint endHint);

	void OpenSceneFilters();
	void OpenFilters();

	void EnablePreviewDisplay(bool enable);
	void TogglePreview();

	void NudgeUp();
	void NudgeDown();
	void NudgeLeft();
	void NudgeRight();

	void OpenStudioProgramProjector();
	void OpenPreviewProjector();
	void OpenSourceProjector();
	void OpenMultiviewProjector();
	void OpenSceneProjector();

	void OpenStudioProgramWindow();
	void OpenPreviewWindow();
	void OpenSourceWindow();
	void OpenMultiviewWindow();
	void OpenSceneWindow();

	void DeferredLoad(const QString &file, int requeueCount);

	void StackedMixerAreaContextMenuRequested();

	void ResizeOutputSizeOfSource();

public slots:
	void on_actionResetTransform_triggered();

	bool StreamingActive();
	bool RecordingActive();
	bool ReplayBufferActive();
	void slotUserPortraitDownloadFinished();
	void onLiveRoomTitleChanged();
	void onHotKeyChanged(QString micHotKey, QString liveHotKey, QString barrageHotKey);

	void onAddMaterialOfCamera();
	void onAddMaterialOfVideo();
	void onAddMaterialOfImage();
	void onAddMaterialOfText();
	void onAddMaterialOfSource();
	void onAddMaterialOfCapture();
	void onAddMaterialOfFullScreen();
	void onAddMaterialOfWindow();
	void onMediaSoundsValChanged(int val, QString sourceName);

	void onShowAboutDlg();
	void onShowSettingsIniDlg();
	void onShowLogoutDlg();
	void onShowFeedbackDlg();
	void onExitApplication();
	void onHandlerExitApp();
	void activated(QSystemTrayIcon::ActivationReason reason);
	void slotAPIVerify();
	void onWaterMarkStateChanged(int state);

public:
	explicit OBSBasic(QWidget *parent = 0);
	virtual ~OBSBasic();

	virtual void OBSInit() override;

	virtual config_t *Config() const override;

	obs_source_t *GetAudioSource() const;

	virtual int GetProfilePath(char *path, size_t size, const char *file)
		const override;

	static void InitBrowserPanelSafeBlock();

	CWaterMark *GetWaterMark()
	{
		return m_pWaterMark;
	}

public:
	std::unique_ptr<Ui::OBSBasic> ui;
	CWaterMark *m_pWaterMark;
	enum EN_DISPLAY_STATE
	{
		kMaximizedState,
		kNormalState
	};

	enum EN_EXIT_TYPE
	{
		kNormalExit,
		kSessionInvalidation,
		kLogout,
		kNetworkConnectionDisconnected
	};

	EN_DISPLAY_STATE m_eDisplayState;
	QString m_qsRTMPUrl;
	QString m_qsKey;
	int m_nGroupNum;
	EN_EXIT_TYPE m_eExitType;
	ST_LIVE_CONTENT m_stDefaultLiveContent;
	int m_nLiveRecordID;
	bool m_bLivetrailerClear = false;
};
