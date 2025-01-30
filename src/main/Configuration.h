// Configuration.h
//
// PASSWORD TECH
// Copyright (c) 2002-2025 by Christian Thoeing <c.thoeing@web.de>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.
//---------------------------------------------------------------------------

#ifndef ConfigurationH
#define ConfigurationH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Dialogs.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Menus.hpp>
#include <System.ImageList.hpp>
#include <Vcl.ImgList.hpp>
#include <map>
#include <vector>
//---------------------------------------------------------------------------
#include "UnicodeUtil.h"

const int
AUTOCLEARCLIPTIME_MIN     = 3,
AUTOCLEARCLIPTIME_MAX     = 32767,
AUTOCLEARCLIPTIME_DEFAULT = 10,
AUTOCLEARPASSWTIME_MIN    = 3,
AUTOCLEARPASSWTIME_MAX    = 32767,
AUTOCLEARPASSWTIME_DEFAULT= 30,

HOTKEYS_MAX_NUM = 10,

DATABASE_MAX_NUM_BACKUPS  = 999;

enum AutoCheckUpdates
{
  acuDaily,
  acuWeekly,
  acuMonthly,
  acuDisabled
};

enum HotKeyAction
{
  hkaNone,
  hkaSinglePassw,
  hkaPasswList,
  hkaPasswClipboard,
  hkaPasswMsgBox,
  hkaPasswAutotype,
  hkaShowMPPG,
  hkaShowPasswManager,
  hkaSearchDatabase,
  hkaSearchDatabaseAutotype
};

enum AutoSaveDatabase
{
  asdEntryModification,
  asdEveryChange
};

enum NewlineChar
{
  nlcWindows,
  nlcUnix
};

extern const std::vector<std::pair<WString, WString>> AppIconNames;

inline WString NewlineCharToString(NewlineChar c)
{
  return (c == nlcWindows) ? "\r\n" : "\n";
}

struct HotKeyEntry {
  HotKeyAction Action;
  bool ShowMainWin;
};

typedef std::map<TShortCut,HotKeyEntry> HotKeyList;

struct LanguageEntry {
  WString Code;
  WString Name;
  WString Version;
  WString FileName;

  LanguageEntry() {}
  LanguageEntry(const WString& sCode) : Code(sCode) {}

  bool operator== (const LanguageEntry& other) const
  {
    return Code == other.Code;
  }
};

struct Configuration {
  WString UiStyleName;
  WString AppIconName;
  WString GUIFontString;
  bool AutoClearClip = true;
  int AutoClearClipTime = AUTOCLEARCLIPTIME_DEFAULT;
  bool AutoClearPassw = true;
  int AutoClearPasswTime = AUTOCLEARPASSWTIME_DEFAULT;
  bool MinimizeAutotype = true;
  int AutotypeDelay = 50;
  bool TestCommonPassw = true;
  bool UseAdvancedPasswEst = true;
  bool ShowSysTrayIconConst = true;
  bool MinimizeToSysTray = true;
  bool ConfirmExit = false;
  bool LaunchSystemStartup = false;
  bool LoadProfileStartup = false;
  WString LoadProfileName;
  int RandomPoolCipher = 1;
  AutoCheckUpdates AutoCheckUpdates = acuWeekly;
  CharacterEncoding FileEncoding = ceUtf8;
  NewlineChar FileNewlineChar = nlcWindows;
  HotKeyList HotKeys;
  LanguageEntry Language;
  //int LanguageIndex = 0;
  struct {
    //bool ClearClipMinimize = true;
    bool ClearClipCloseLock = true;
    bool LockMinimize = true;
    bool LockIdle = true;
    int LockIdleTime = 300;
    bool LockAutoSave = false;
    bool CreateBackup = true;
    bool NumberBackups = true;
    bool OpenWindowOnStartup = false;
    bool OpenLastDbOnStartup = true;
    bool WarnExpiredEntries = false;
    bool WarnEntriesExpireSoon = false;
    int WarnExpireNumDays = 7;
    int MaxNumBackups = 5;
    bool AutoSave = false;
    AutoSaveDatabase AutoSaveOption = asdEntryModification;
    WString DefaultAutotypeSequence;
    bool KeepRecentFiles = true;
  } Database;
};


class TConfigurationDlg : public TForm
{
__published:	// IDE-managed Components
  TPageControl *ConfigPages;
  TTabSheet *GeneralSheet;
  TTabSheet *UpdatesSheet;
  TLabel *ChangeFontLbl;
  TButton *SelectFontBtn;
  TCheckBox *ShowSysTrayIconConstCheck;
  TCheckBox *MinimizeToSysTrayCheck;
  TTabSheet *HotKeySheet;
  THotKey *HotKeyBox;
  TGroupBox *HotKeyActionsGroup;
  TCheckBox *HotKeyShowMainWinCheck;
  TComboBox *HotKeyActionsList;
  TButton *OKBtn;
  TButton *CancelBtn;
  TFontDialog *FontDlg;
  TLabel *FontSampleLbl;
  TTabSheet *FilesSheet;
  TLabel *HotKeyLbl;
  TListView *HotKeyView;
  TButton *AddBtn;
  TButton *RemoveBtn;
  TTabSheet *LanguageSheet;
  TLabel *SelectLanguageLbl;
  TComboBox *LanguageList;
  TTabSheet *DatabaseSheet;
  TCheckBox *LockMinimizeCheck;
  TCheckBox *LockIdleCheck;
  TEdit *LockIdleTimeBox;
    TUpDown *LockIdleTimeSpinBtn;
  TCheckBox *CreateBackupCheck;
  TEdit *MaxNumBackupsBox;
    TUpDown *MaxNumBackupsSpinBtn;
  TCheckBox *OpenDbOnStartupCheck;
    TCheckBox *ClearClipCloseLockCheck;
  TCheckBox *OpenWindowOnStartupCheck;
  TCheckBox *LockAutoSaveCheck;
    TTabSheet *SecuritySheet;
    TCheckBox *AutoClearClipCheck;
    TEdit *AutoClearClipTimeBox;
    TUpDown *AutoClearClipTimeSpinBtn;
    TCheckBox *TestCommonPasswCheck;
    TLabel *RandomPoolCipherLbl;
    TComboBox *RandomPoolCipherList;
    TLabel *AutotypeDelayLbl;
    TEdit *AutotypeDelayBox;
    TUpDown *AutotypeDelaySpinBtn;
  TLabel *DefaultAutotypeSeqLbl;
    TEdit *DefaultAutotypeSeqBox;
    TCheckBox *MinimizeAutotypeCheck;
    TCheckBox *AskBeforeExitCheck;
    TCheckBox *AutoClearPasswCheck;
    TUpDown *AutoClearPasswTimeSpinBtn;
    TEdit *AutoClearPasswTimeBox;
  TCheckBox *NumberBackupsCheck;
  TCheckBox *AutoSaveCheck;
  TComboBox *AutoSaveList;
  TCheckBox *WarnExpiredEntriesCheck;
    TCheckBox *LaunchSystemStartupCheck;
    TButton *BenchmarkBtn;
    TCheckBox *WarnEntriesExpireSoonCheck;
    TLabel *WarnExpireNumDaysLbl;
    TEdit *WarnExpireNumDaysBox;
    TUpDown *WarnExpireNumDaysSpinBtn;
    TButton *ConvertLangFileBtn;
    TSaveDialog *SaveDlg;
    TCheckBox *UseAdvancedPasswEst;
    TLabel *UiStyleLbl;
    TComboBox *UiStylesList;
    TRadioGroup *UpdateCheckGroup;
    TRadioGroup *CharEncodingGroup;
    TRadioGroup *NewlineCharGroup;
    TComboBox *BenchmarkMemList;
    TPopupMenu *SelectFontMenu;
    TMenuItem *SelectFontMenu_RestoreDefault;
    TCheckBox *LoadProfileStartupCheck;
    TComboBox *LoadProfileBox;
  TComboBoxEx *AppIconList;
  TLabel *AppIconLbl;
  TImageList *AppIconImageList;
  TButton *InstallLanguageBtn;
  TOpenDialog *OpenDlg;
  TButton *RemoveLanguageBtn;
  TCheckBox *KeepRecentFilesCheck;
  void __fastcall SelectFontBtnClick(TObject *Sender);
  void __fastcall AutoClearClipCheckClick(TObject *Sender);
  void __fastcall FormShow(TObject *Sender);
  void __fastcall OKBtnClick(TObject *Sender);
  void __fastcall HotKeyBoxChange(TObject *Sender);
  void __fastcall AddBtnClick(TObject *Sender);
  void __fastcall HotKeyViewSelectItem(TObject *Sender,
    TListItem *Item, bool Selected);
  void __fastcall RemoveBtnClick(TObject *Sender);
  void __fastcall LockIdleCheckClick(TObject *Sender);
  void __fastcall CreateBackupCheckClick(TObject *Sender);
  void __fastcall AutoClearPasswCheckClick(TObject *Sender);
  void __fastcall NumberBackupsCheckClick(TObject *Sender);
    void __fastcall AutoSaveCheckClick(TObject *Sender);
    void __fastcall BenchmarkBtnClick(TObject *Sender);
    void __fastcall ConvertLangFileBtnClick(TObject *Sender);
    void __fastcall LanguageListSelect(TObject *Sender);
    void __fastcall SelectFontMenu_RestoreDefaultClick(TObject *Sender);
    void __fastcall LoadProfileStartupCheckClick(TObject *Sender);
  void __fastcall InstallLanguageBtnClick(TObject *Sender);
  void __fastcall RemoveLanguageBtnClick(TObject *Sender);
private:	// User declarations
  HotKeyList m_hotKeys;
  //std::vector<LanguageEntry> m_langList;
  void __fastcall ShowFontSample(TFont* pFont);
  void __fastcall UpdateHotKeyList(void);
  void __fastcall UpdateProfileList(void);
public:		// User declarations
  __fastcall TConfigurationDlg(TComponent* Owner);
  void __fastcall LoadConfig(void);
  void __fastcall SaveConfig(void);
  void __fastcall GetOptions(Configuration& config);
  void __fastcall SetOptions(const Configuration& config);
  void __fastcall LoadLanguages(void);
  void __fastcall SetDonorUI(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TConfigurationDlg *ConfigurationDlg;
//---------------------------------------------------------------------------
#endif
