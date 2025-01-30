// SendKeys.cpp
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
#include <unordered_map>
#include <vcl.h>
#pragma hdrstop

#include "SendKeys.h"
#include "MemUtil.h"
#include "Progress.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

static std::unordered_map<std::string, int> s_keyPlaceholders;

std::atomic<int> TSendKeysThread::s_nThreadState(TSendKeysThread::INACTIVE);

const int
  VIRTUAL_KEY_DELAY = 200,
  INIT_DELAY        = 250;

SendKeys::KeySequence::~KeySequence()
{
  for (auto& k : keys)
    eraseVector(k);
  eraseVector(delays);
}
//---------------------------------------------------------------------------
SendKeys::SendKeys(int nDelay)
  : m_nDelay(nDelay)
{
  if (s_keyPlaceholders.empty()) {
    for (int i = 0; i < PasswDbEntry::NUM_STRING_FIELDS; i++) {
      AnsiString sFieldName = AnsiString(PasswDbEntry::GetFieldName(
        static_cast<PasswDbEntry::FieldType>(i))).LowerCase();
      s_keyPlaceholders[std::string(sFieldName.c_str())] = -i;
    }

    s_keyPlaceholders["parameter"] = -PasswDbEntry::USERNAME;

    s_keyPlaceholders["tab"] = VK_TAB;
    s_keyPlaceholders["return"] = VK_RETURN;
    s_keyPlaceholders["enter"] = VK_RETURN;
    s_keyPlaceholders["ctrl"] = VK_CONTROL;
    s_keyPlaceholders["backspace"] = VK_BACK;
    s_keyPlaceholders["clear"] = VK_CLEAR;
    s_keyPlaceholders["shift"] = VK_SHIFT;
    s_keyPlaceholders["alt"] = VK_MENU;
    s_keyPlaceholders["pause"] = VK_PAUSE;
    s_keyPlaceholders["capslock"] = VK_CAPITAL;
    s_keyPlaceholders["escape"] = VK_ESCAPE;
    s_keyPlaceholders["space"] = VK_SPACE;
    s_keyPlaceholders["pageup"] = VK_PRIOR;
    s_keyPlaceholders["pagedown"] = VK_NEXT;
    s_keyPlaceholders["end"] = VK_END;
    s_keyPlaceholders["home"] = VK_HOME;
    s_keyPlaceholders["left"] = VK_LEFT;
    s_keyPlaceholders["right"] = VK_RIGHT;
    s_keyPlaceholders["down"] = VK_DOWN;
    s_keyPlaceholders["up"] = VK_UP;
    s_keyPlaceholders["select"] = VK_SELECT;
    s_keyPlaceholders["print"] = VK_PRINT;
    s_keyPlaceholders["execute"] = VK_EXECUTE;
    s_keyPlaceholders["snapshot"] = VK_SNAPSHOT;
    s_keyPlaceholders["insert"] = VK_INSERT;
    s_keyPlaceholders["delete"] = VK_DELETE;
    s_keyPlaceholders["help"] = VK_HELP;
  }
}
//---------------------------------------------------------------------------
const wchar_t* SendKeys::SendUnicodeChar(const wchar_t* pwszKeyPair,
  std::vector<INPUT>* pDest)
{
  std::vector<INPUT> vi;

  INPUT ip;
  ip.type = INPUT_KEYBOARD;
  ip.ki.time = 0;
  ip.ki.dwExtraInfo = 0;
  ip.ki.wVk = 0;

  if (*pwszKeyPair < 0xd800 || *pwszKeyPair > 0xdbff) {
    // first down, then up
    ip.ki.dwFlags = KEYEVENTF_UNICODE;
    ip.ki.wScan = *pwszKeyPair++;
    vi.push_back(ip);
    ip.ki.dwFlags |= KEYEVENTF_KEYUP;
    vi.push_back(ip);
  }
  else {
    // first both parts of the unicode char down
    ip.ki.dwFlags = KEYEVENTF_UNICODE;
    ip.ki.wScan = pwszKeyPair[0];
    vi.push_back(ip);
    ip.ki.wScan = pwszKeyPair[1];
    vi.push_back(ip);

    // then both parts up
    ip.ki.dwFlags |= KEYEVENTF_KEYUP;
    ip.ki.wScan = pwszKeyPair[0];
    vi.push_back(ip);
    ip.ki.wScan = pwszKeyPair[1];
    vi.push_back(ip);

    pwszKeyPair += 2;
  }

  if (pDest != nullptr)
    *pDest = vi;
  else
    SendInput(vi.size(), vi.data(), sizeof(INPUT));

  eraseVector(vi);
  memzero(&ip, sizeof(INPUT));

  return pwszKeyPair;
}
//---------------------------------------------------------------------------
void SendKeys::SendString(const wchar_t* pwszStr)
{
  Sleep(INIT_DELAY);
  while (*pwszStr != '\0' && !TSendKeysThread::ThreadAborted()) {
    pwszStr = SendUnicodeChar(pwszStr);
    Sleep(m_nDelay);
  }
}
//---------------------------------------------------------------------------
void SendKeys::AddString(const wchar_t* pwszStr, KeySequence& dest)
{
  while (*pwszStr != '\0') {
    std::vector<INPUT> vi;
    pwszStr = SendUnicodeChar(pwszStr, &vi);
    dest.keys.push_back(vi);
    dest.delays.push_back(m_nDelay);
    eraseVector(vi);
  }
}
//---------------------------------------------------------------------------
void SendKeys::SendVirtualKey(word16 wKey,
  std::vector<word16>* pAddKeys,
  std::vector<INPUT>* pDest)
{
  std::vector<INPUT> vi;
  if (pAddKeys != nullptr) {
    for (auto ch : *pAddKeys)
      AddVirtualKey(vi, ch, true);
  }
  AddVirtualKey(vi, wKey, true);
  AddVirtualKey(vi, wKey, false);
  if (pAddKeys != nullptr) {
    for (auto ch : *pAddKeys)
      AddVirtualKey(vi, ch, false);
    eraseVector(*pAddKeys);
  }

  if (pDest != nullptr)
    *pDest = vi;
  else
    SendInput(vi.size(), vi.data(), sizeof(INPUT));

  eraseVector(vi);
}
//---------------------------------------------------------------------------
void SendKeys::AddVirtualKey(std::vector<INPUT>& dest, word16 wKey, bool blDown)
{
  INPUT ip;
  ip.type = INPUT_KEYBOARD;
  ip.ki.time = 0;
  ip.ki.dwExtraInfo = 0;
  ip.ki.wScan = 0;
  ip.ki.wVk = wKey;
  ip.ki.dwFlags = blDown ? 0 : KEYEVENTF_KEYUP;
  dest.push_back(ip);
  memzero(&ip, sizeof(INPUT));
}
//---------------------------------------------------------------------------
void SendKeys::SendComplexString(const WString& sStr,
  const PasswDbEntry* pPasswDbEntry,
  std::shared_ptr<PasswDatabase> pPasswDb,
  const wchar_t* pwszParam,
  const wchar_t* pwszPassw,
  KeySequence* pDest)
{
  const wchar_t* pwszStr = sStr.c_str();
  std::vector<word16> bufferedKeys;
  std::vector<INPUT> vi;

  while (*pwszStr != '\0') {
    bool blFound = false;

    if (*pwszStr == '{') {
      const wchar_t* pwszEnd = wcschr(pwszStr + 1, '}');
      if (pwszEnd != nullptr && pwszEnd != pwszStr + 1) {
        std::string sPlaceholder;
        sPlaceholder.reserve(static_cast<size_t>(pwszEnd - pwszStr) + 1);
        const wchar_t* pwszStart = pwszStr + 1;
        while (*pwszStart != *pwszEnd)
          sPlaceholder.push_back(tolower(*pwszStart++));

        if (sPlaceholder.length() > 5 &&
            sPlaceholder.compare(0, 5, "wait:") == 0)
        {
          blFound = true;
          pwszStr += sPlaceholder.length() + 2;
          try {
            int nTime = std::max(10, std::min(10000, std::stoi(sPlaceholder.substr(5))));
            if (pDest) {
              pDest->keys.push_back(std::vector<INPUT>());
              pDest->delays.push_back(nTime);
            }
            else {
              Sleep(nTime);
            }
          }
          catch (...)
          {}
        }
        else {
          auto it = s_keyPlaceholders.find(sPlaceholder);
          if (it != s_keyPlaceholders.end()) {
            blFound = true;
            pwszStr += it->first.length() + 2; // including brackets

            // found placeholder related to password database or MPPG
            if (it->second <= 0) {
              int nIdx = -it->second;

              if (pPasswDbEntry && pPasswDb) {
                const SecureWString* psSrc;
                SecureWString sPassw;
                if (nIdx == PasswDbEntry::PASSWORD && !pPasswDbEntry->HasPlaintextPassw()) {
                  sPassw = pPasswDb->GetDbEntryPassw(*pPasswDbEntry);
                  psSrc = &sPassw;
                }
                else
                  psSrc = &pPasswDbEntry->Strings[nIdx];
                if (!psSrc->IsStrEmpty()) {
                  if (pDest != nullptr)
                    AddString(psSrc->c_str(), *pDest);
                  else
                    SendString(psSrc->c_str());
                }
              }
              else {
                // "parameter" corresponds to "user name"
                if (nIdx == PasswDbEntry::USERNAME && pwszParam != nullptr) {
                  if (pDest != nullptr)
                    AddString(pwszParam, *pDest);
                  else
                    SendString(pwszParam);
                }
                else if (nIdx == PasswDbEntry::PASSWORD && pwszPassw != nullptr) {
                  if (pDest != nullptr)
                    AddString(pwszPassw, *pDest);
                  else
                    SendString(pwszPassw);
                }
              }
            }

            // found placeholder for virtual key
            else {
              word16 wKey = it->second;
              int nDelay = m_nDelay;

              switch (it->second) {
              case VK_CONTROL:
              case VK_MENU:
              case VK_SHIFT:
                if (bufferedKeys.size() < 3)
                  bufferedKeys.push_back(wKey);
                wKey = 0;
                break;

              case VK_TAB:
              case VK_RETURN:
                nDelay = std::max(VIRTUAL_KEY_DELAY, nDelay);
                break;
              }

              if (wKey > 0) {
                if (pDest != nullptr) {
                  SendVirtualKey(wKey, &bufferedKeys, &vi);
                  pDest->keys.push_back(vi);
                  if (!pDest->delays.empty())
                    pDest->delays.back() = std::max(VIRTUAL_KEY_DELAY, pDest->delays.back());
                  pDest->delays.push_back(nDelay);
                  eraseVector(vi);
                }
                else {
                  SendVirtualKey(wKey, &bufferedKeys);
                  Sleep(nDelay);
                }
                wKey = 0;
              }
            }
          }
        }
      }
    }

    if (!blFound) {
      if (bufferedKeys.empty())
        pwszStr = SendUnicodeChar(pwszStr, pDest ? &vi : nullptr);
      else
        SendVirtualKey(*pwszStr++, &bufferedKeys, pDest ? &vi : nullptr);
      if (pDest != nullptr) {
        pDest->keys.push_back(vi);
        pDest->delays.push_back(m_nDelay);
        eraseVector(vi);
      }
      else
        Sleep(m_nDelay);
    }
  }

  eraseVector(vi);
  eraseVector(bufferedKeys);
}
//---------------------------------------------------------------------------
void SendKeys::SendKeySequence(KeySequence& input)
{
  Sleep(INIT_DELAY);
  for (word32 i = 0; i < input.keys.size() && !TSendKeysThread::ThreadAborted(); i++) {
    if (!input.keys[i].empty())
      SendInput(input.keys[i].size(), input.keys[i].data(), sizeof(INPUT));
    Sleep(input.delays[i]);
  }
}
//---------------------------------------------------------------------------

void __fastcall TSendKeysThread::Execute(void)
{
  int nTimeout = 0;
  while (s_nThreadState != ABORTED && nTimeout < 10000) {
    HWND hWin = GetForegroundWindow();
    if (hWin != Application->Handle && hWin != m_hSender &&
        hWin != ProgressForm->Handle) {
      if (!m_sStr.IsEmpty())
        m_sendKeys.SendString(m_sStr.c_str());
      else
        m_sendKeys.SendKeySequence(m_keySeq);
      break;
    }
    TThread::Sleep(100);
    nTimeout += 100;
  }
}
//---------------------------------------------------------------------------
void __fastcall TSendKeysThread::TerminateAndWait(int nTimeout)
{
  TSendKeysThread::TerminateActiveThread();
  int nWait = 0;
  while (ThreadRunning() && nWait < nTimeout) {
    TThread::Sleep(100);
    nWait += 100;
    Application->ProcessMessages();
  }
}
