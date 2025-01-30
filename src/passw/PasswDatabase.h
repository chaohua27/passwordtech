// PasswDatabase.h
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
#ifndef PasswDatabaseH
#define PasswDatabaseH
//---------------------------------------------------------------------------
#include <vector>
#include <memory>
#include <set>
#include <Classes.hpp>
#include "UnicodeUtil.h"
#include "SecureMem.h"
#include "DataCompressor.h"
#include "SymmetricCipher.h"
#include "RandomGenerator.h"

// class for password database entry
class PasswDbEntry {
public:
  friend class PasswDatabase;

  using KeyValue = std::pair<SecureWString,SecureWString>;
  using KeyValueList = std::vector<KeyValue>;
  using PasswHistoryEntry = std::pair<FILETIME,SecureWString>;

  class PasswHistory {
  public:
    PasswHistory() : m_lMaxSize(1), m_blActive(false) {}

    PasswHistory(word32 lMaxSize, bool blActive)
      : m_lMaxSize(std::max(1u, lMaxSize)), m_blActive(blActive)
    {}

    bool IsEmpty(void) const
    {
      return !m_blActive && m_history.empty();
    }

    word32 GetSize(void) const
    {
      return m_history.size();
    }

    word32 GetMaxSize(void) const
    {
      return m_lMaxSize;
    }

    bool GetActive(void) const
    {
      return m_blActive;
    }

    void SetMaxSize(word32 lMaxSize)
    {
      m_lMaxSize = std::max(1u, lMaxSize);
      if (m_history.size() > m_lMaxSize)
        m_history.erase(m_history.begin() + m_lMaxSize, m_history.end());
    }

    void SetActive(bool blActive)
    {
      m_blActive = blActive;
    }

    /*const std::vector<PasswHistoryEntry>& GetHistory(void) const
    {
      return m_history;
    }*/

    std::vector<PasswHistoryEntry>::const_iterator begin() const
    {
      return m_history.cbegin();
    }

    std::vector<PasswHistoryEntry>::const_iterator end() const
    {
      return m_history.cend();
    }

    void ClearHistory(void)
    {
      m_history.clear();
    }

    void AdoptFrom(const PasswHistory& src)
    {
      SetActive(src.m_blActive);
      if (src.GetSize() == 0)
        ClearHistory();
      SetMaxSize(src.m_lMaxSize);
    }

    void AddEntry(const PasswHistoryEntry& entry, bool blToFront = true);

  private:
    std::vector<PasswHistoryEntry> m_history;
    word32 m_lMaxSize;
    bool m_blActive;
  };

  enum {
    NUM_FIELDS = 13,
    NUM_STRING_FIELDS = 8
  };

  enum FieldType {
    TITLE,
    USERNAME,
    PASSWORD,
    URL,
    KEYWORD,
    NOTES,
    KEYVALUELIST,
    TAGS,
    CREATIONTIME,
    MODIFICATIONTIME,
    PASSWCHANGETIME,
    PASSWEXPIRYDATE,
    PASSWHISTORY,

    END = 0xff
  };

  SecureWString Strings[NUM_STRING_FIELDS];
  SecureWString CreationTimeString;
  SecureWString ModificationTimeString;
  SecureWString PasswChangeTimeString;
  SecureWString PasswExpiryDateString;
  FILETIME CreationTime;
  FILETIME ModificationTime;
  FILETIME PasswChangeTime;
  word32 PasswExpiryDate;
  word32 UserFlags;
  int UserTag;

  ~PasswDbEntry()
  {
    memzero(&CreationTime, sizeof(CreationTime));
    memzero(&ModificationTime, sizeof(ModificationTime));
    PasswExpiryDate = 0;
    UserFlags = 0;
    UserTag = 0;
  }

  // get unique 32-bit identifier
  word32 GetId(void) const
  {
    return m_lId;
  }

  // get index of entry within database
  word32 GetIndex(void) const
  {
    return m_lIndex;
  }

  // check if password is empty
  bool IsPasswEmpty(void) const
  {
    return m_encPassw.IsEmpty();
  }

  // password can be stored as plaintext or in encrypted form;
  // check if plaintext password is available
  bool HasPlaintextPassw(void) const
  {
    return !m_encPassw.IsEmpty() && !Strings[PASSWORD].IsEmpty();
  }

  // update timestamp of last modification
  void UpdateModificationTime(bool blPasswChanged)
  {
    SYSTEMTIME st;
    GetLocalTime(&st);
    SystemTimeToFileTime(&st, &ModificationTime);
    ModificationTimeString = TimeStampToString(ModificationTime);
    if (blPasswChanged) {
      PasswChangeTime = ModificationTime;
      PasswChangeTimeString = ModificationTimeString;
    }
  }

  // access to key-value list
  const KeyValueList& GetKeyValueList(void) const
  {
    return m_keyValueList;
  }

  void SetKeyValueList(const KeyValueList& src)
  {
    m_keyValueList = src;
    UpdateKeyValueString();
  }

  // get/set "value" item corresponding to "key" item
  const SecureWString* GetKeyValue(const wchar_t* pwszKey) const;
  void SetKeyValue(const wchar_t* pwszKey, const wchar_t* pwszValue);

  // get key-value list as single string, with key-value pairs separated
  // by "sep" character
  SecureWString GetKeyValueListAsString(wchar_t sep = '\n') const;

  // clear (empty) key-value list
  void ClearKeyValueList(void)
  {
    m_keyValueList.clear();
    Strings[KEYVALUELIST].Clear();
  }

  // update internal key-value list string, key-value pairs separated by ','
  void UpdateKeyValueString(void)
  {
    Strings[KEYVALUELIST] = GetKeyValueListAsString(',');
  }

  // get/set list of tags
  const std::set<SecureWString>& GetTagList(void) const
  {
    return m_tags;
  }

  void SetTagList(const std::set<SecureWString>& tags)
  {
    m_tags = tags;
    UpdateTagsString();
  }

  // check if specified tag is available
  bool CheckTag(const SecureWString& sTag) const;

  // add tag to entry
  // <- 'true' if tag was added successfully, 'false' if tag already exists
  bool AddTag(const SecureWString& sTag);

  // get list of tags as single string, with tags separated by "sep" character
  SecureWString GetTagsAsString(wchar_t sep = '\n') const;

  // update internal list of tags, tags separated by ','
  void UpdateTagsString(void)
  {
    Strings[TAGS] = GetTagsAsString(',');
  }

  // clear (empty) list of tags
  void ClearTagList(void)
  {
    m_tags.clear();
    Strings[TAGS].Clear();
  }

  PasswHistory& GetPasswHistory(void)
  {
    return m_passwHistory;
  }

  const PasswHistory& GetPasswHistory(void) const
  {
    return m_passwHistory;
  }

  void AddCurrentPasswToHistory(const SecureWString& sPassw)
  {
    m_passwHistory.AddEntry({ PasswChangeTime, sPassw });
  }

  // convert timestamp to string using Windows API
  static SecureWString TimeStampToString(const FILETIME& ft);

  // convert expiry date to string
  static SecureWString ExpiryDateToString(word32 lDate);

  // decode 32-bit expiry data into year/month/date format
  static bool DecodeExpiryDate(word32 lDate, int& nYear, int& nMonth, int& nDay)
  {
    nYear = lDate >> 16;
    nMonth = (lDate >> 8) & 0xff;
    nDay = lDate & 0xff;
    return nYear > 0 && nMonth >= 1 && nMonth <= 12 && nDay >= 1 && nDay <= 31;
  }

  // encode year/month/date specification into 32-bit expiry date
  static word32 EncodeExpiryDate(int nYear, int nMonth, int nDay)
  {
    return (nYear << 16) | (nMonth << 8) | nDay;
  }

  // get field name of field type as string
  static const char* GetFieldName(FieldType type);

private:
  // private constructor
  // -> unique 32-bit identifier
  // -> index of entry within database
  // -> 'true': set "creation" and "last modification" timestamps
  PasswDbEntry(word32 lId, word32 lIndex, bool blSetTimeStamps,
    word32 lMaxPasswHistorySize, bool blPasswHistoryActive)
    : m_lId(lId), m_lIndex(lIndex), m_passwHash(20), UserFlags(0), UserTag(0),
    PasswExpiryDate(0), m_passwHistory(lMaxPasswHistorySize, blPasswHistoryActive)
  {
    if (blSetTimeStamps) {
      SYSTEMTIME st;
      GetLocalTime(&st);
      SystemTimeToFileTime(&st, &CreationTime);
      CreationTimeString = TimeStampToString(CreationTime);
      ModificationTime = CreationTime;
      ModificationTimeString = CreationTimeString;
    }
    else {
      CreationTime.dwLowDateTime = CreationTime.dwHighDateTime = 0;
      ModificationTime.dwLowDateTime = ModificationTime.dwHighDateTime = 0;
    }
    PasswChangeTime.dwLowDateTime = PasswChangeTime.dwHighDateTime = 0;
  }

  // parse key-value list specified as string
  void ParseKeyValueList(const SecureWString& sList);

  // parse list of tags specified as string
  void ParseTagList(const SecureWString& sList);

  word32 m_lId;
  word32 m_lIndex;
  word32 m_lMaxPasswHistorySize;
  SecureMem<wchar_t> m_encPassw;
  SecureMem<word8> m_passwHash;
  std::vector<KeyValue> m_keyValueList;
  std::set<SecureWString> m_tags;
  PasswHistory m_passwHistory;
};


typedef std::vector<std::unique_ptr<PasswDbEntry>> PasswDbList;


// base class for exceptions related to password databases
class EPasswDbError : public Exception
{
public:
  __fastcall EPasswDbError(const WString& sMsg)
    : Exception(sMsg)
  {}
};

class EPasswDbInvalidFormat : public EPasswDbError
{
public:
  __fastcall EPasswDbInvalidFormat(const WString& sMsg)
    : EPasswDbError(sMsg)
  {}
};

// db not encrypted, invalid key, or authentication error (HMAC mismatch)
class EPasswDbInvalidKey : public EPasswDbError
{
public:
  __fastcall EPasswDbInvalidKey(const WString& sMsg)
    : EPasswDbError(sMsg)
  {}
};

const WString E_INVALID_FORMAT = "Invalid/unknown file format";

class PasswDatabase {
private:
  enum class DbOpenState {
    Closed,
    Incomplete, // database could not be opened successfully
    Open        // database successfully opened
  };

  enum {
    SECMEM_SIZE = 4096,
    SECMEM_KEY_LENGTH = 32,
    SECMEM_SALT_LENGTH = 16,
    SECMEM_IV_LENGTH = 8,

    DB_KEY_LENGTH = 32,
    DB_SALT_LENGTH = 32,
    SHA256_HMAC_LENGTH = 32,
    SHA512_HMAC_LENGTH = 64,

    DB_RECOVERY_KEY_BLOCK_LENGTH = 2 * (DB_SALT_LENGTH + DB_KEY_LENGTH),

    HASH_SHA256 = 0,
    HASH_SHA512 = 1,

    KDF_PBKDF2_SHA256 = 0,
  };

  PasswDbList m_db;
  int m_nLastVersion;
  word8 m_bCipherType;
  word32 m_lKdfIterations;
  word32 m_lDbEntryId;
  word8* m_pSecMem;
  chacha_ctx* m_pMemCipherCtx;
  word8* m_pMemSalt;
  word8* m_pDbSalt;
  word8* m_pDbKey;
  word8* m_pDbRecoveryKeyBlock;
  SecureMem<word8> m_cryptBuf;
  word32 m_lCryptBufPos;
  std::unique_ptr<TFileStream> m_pFile;
  SecureWString m_sDefaultUserName;
  SecureWString m_sPasswFormatSeq;
  word32 m_lDefaultPasswExpiryDays;
  word32 m_lDefaultMaxPasswHistorySize;
  bool m_blPlaintextPassw;
  DbOpenState m_dbOpenState;
  bool m_blRecoveryKey;
  bool m_blCompressed;
  int m_nCompressionLevel;

  // initializes crypto engine (encryption and hash algorithms),
  // allocates RAM to protect the database master key and passwords
  // -> master key
  void Initialize(const SecureMem<word8>& key);

  // throws exception if database is already open
  void CheckDbNotOpen(void);

  // throw exception if database is not open
  void CheckDbOpen(void) const;

  // creates new instance of a cipher
  // -> cipher type
  // -> pointer to 256-bit key
  // -> cipher mode (encryption/decryption)
  std::unique_ptr<EncryptionAlgorithm::SymmetricCipher> CreateCipher(
    int nType, const word8* pKey, EncryptionAlgorithm::Mode mode);

  // write buffer contents to file
  // -> buffer of any type
  // -> number of bytes to write
  void Write(const void* pBuf, word32 lNumOfBytes);

  // write field data (Title, User name, ...) to file
  // -> data buffer
  // -> number of bytes
  // -> index of field (<0: index not applicable)
  void WriteFieldBuf(const void* pBuf, word32 lNumOfBytes, int nIndex = -1);

  // template function for writing field data
  template<typename T> void WriteField(const T& t, int nIndex = -1)
  {
    WriteFieldBuf(&t, sizeof(T), nIndex);
  }

  // template function for writing data type to file
  template<typename T> void WriteType(const T& t)
  {
    Write(&t, sizeof(T));
  }

  // write ASCII (non-Unicode) string to file
  // -> string buffer (must be zero-terminated if length is -1)
  // -> string length (will be determined if value of -1 is specified)
  // -> index of field (<0: index not applicable)
  void WriteString(const char* pszStr, word32 lLen = -1, int nIndex = -1);

  // write Unicode string to file
  // -> string
  // -> index of field (<0: index not applicable)
  void WriteString(const SecureWString& sStr, int nIndex = -1);

  // read index of field
  int ReadFieldIndex(void);

  // read size of field
  word32 ReadFieldSize(void);

  // read ASCII (non-Unicode) string
  SecureAnsiString ReadAnsiString(void);

  // read Unicode string
  SecureWString ReadString(void);

  // template function for reading field of a specific type
  template<typename T> T ReadField(void)
  {
    word32 lSize = ReadFieldSize();
    if (lSize != sizeof(T))
      throw EPasswDbInvalidFormat(E_INVALID_FORMAT);
    T t;
    memcpy(&t, &m_cryptBuf[m_lCryptBufPos], lSize);
    m_lCryptBufPos += lSize;
    return t;
  }

  // template function for reading a specific data type
  template<typename T> T ReadType(void)
  {
    if (m_lCryptBufPos + sizeof(T) > m_cryptBuf.Size())
      throw EPasswDbInvalidFormat(E_INVALID_FORMAT);
    T t;
    memcpy(&t, &m_cryptBuf[m_lCryptBufPos], sizeof(T));
    m_lCryptBufPos += sizeof(T);
    return t;
  }

  // skip current field
  void SkipField(void);

  // returns version number of last opened/saved database
  int GetLastVersion(void)
  {
    return m_nLastVersion;
  }

  // throws exception if recovery key is set
  void CheckCryptoParam(void)
  {
    if (m_blRecoveryKey)
      throw EPasswDbError("Action not allowed if recovery key is set");
  }

  // changes cipher type - must not be called if recovery key is set
  void SetCipherType(word8 bType)
  {
    CheckCryptoParam();
    if (bType > CIPHER_CHACHA20)
      throw EPasswDbError("Cipher not supported");
    m_bCipherType = bType;
  }

  // changes number of KDF iterations - must not be called if recovery key is set
  void SetKdfIterations(word32 lIter)
  {
    CheckCryptoParam();
    if (lIter == 0)
      throw EPasswDbError("Invalid number of KDF iterations");
    m_lKdfIterations = lIter;
  }

  // adds an existing entry from the database file
  PasswDbEntry* AddDbEntry(void);

public:

  enum {
    VERSION_HIGH = 1,
    VERSION_LOW = 5,
    VERSION = (VERSION_HIGH << 8) | VERSION_LOW,

    KEY_HASH_ITERATIONS = 16384,

    CIPHER_AES256 = 0,
    CIPHER_CHACHA20 = 1,

    COMPRESSION_DEFLATE = 1,

    MAX_PASSW_HISTORY_SIZE = 0xff
  };

  // creation/destruction
  PasswDatabase();
  virtual ~PasswDatabase();

  // creates new database
  // -> master key
  void New(const SecureMem<word8>& key);

  // opens existing database file
  // -> master key
  // -> name of file to be opened
  void Open(const SecureMem<word8>& key,
    const WString& sFileName);

  // closes database, wipes and free memory reserved for crypto engine
  void Close(void);

  // checks whether database is open
  bool IsOpen(void) const
  {
    return m_dbOpenState == DbOpenState::Open;
  }

  // saves database to file
  void SaveToFile(const WString& sFileName);

  // release write protection of opened database file
  void ReleaseFile(void)
  {
    m_pFile.reset();
  }

  // gets list of database entries
  /*const PasswDbList& GetDatabase(void) const
  {
    return m_db;
  }*/

  // gets number of database entries
  word32 GetSize(void) const
  {
    return m_db.size();
  }

  // adds a new entry to the database
  PasswDbEntry* NewDbEntry(void);

  // add a duplicate of an existing entry
  // -> original entry to be duplicated
  // -> title of the duplicate
  PasswDbEntry* DuplicateDbEntry(const PasswDbEntry& original,
    const SecureWString& sTitle);

  // removes entry from database
  void DeleteDbEntry(PasswDbEntry& pEntry);

  // changes position of an entry within the database
  // -> current position
  // -> desired new position
  void MoveDbEntry(word32 lCurrPos, word32 lNewPos);

  // changes password of a given database entry
  // -> database entry
  // <- password
  void SetDbEntryPassw(PasswDbEntry& entry,
    const SecureWString& sPassw);

  // returns password of database entry
  // -> database entry
  SecureWString GetDbEntryPassw(const PasswDbEntry& entry);

  // determines whether passwords of all entries are stored in plaintext
  // or ciphertext format in memory (encrypted with a key stored in RAM)
  void SetPlaintextPassw(bool blPlaintextPassw);

  // checks validity of supplied master key
  // <- true/false: key valid/invalid
  bool CheckMasterKey(const SecureMem<word8>& key);

  // checks validity of supplied recovery key
  // <- true/false: key valid/invalid
  bool CheckRecoveryKey(const SecureMem<word8>& recoveryKey);

  // changes master key
  // -> new master key
  // -> new setting for number of KDF iterations
  //    (will only be adopted if operation is successful/not canceled)
  //    0 = use current database setting
  // -> pointer to flag for cancelling operation (if number of iterations
  //    is too high)
  // -> pointer to thread-safe random generator instance (in case function
  //    is called from separate thread)
  void ChangeMasterKey(const SecureMem<word8>& newKey,
    word32 lKdfIterOverride = 0,
    std::atomic<bool>* pCancelFlag = nullptr,
    RandomGenerator* pThreadSafeRandGen = nullptr);

  // sets recovery key
  // -> master key (new or old)
  // -> recovery key
  void SetRecoveryKey(const SecureMem<word8>& key,
    const SecureMem<word8>& recoveryKey);

  // removes/unsets recovery key
  // -> new master key
  void RemoveRecoveryKey(const SecureMem<word8>& key);

  // exports database entries to CSV format (plaintext)
  // -> name of CSV file
  // -> fields/columns to export (bit field)
  // -> names of fields/columns
  void ExportToCsv(const WString& sFileName, int nColMask, const WString* pColNames);

  // creates 256-bit key file, key stored in hexadecimal format (64 bytes)
  // -> name of key file
  static void CreateKeyFile(const WString& sFileName);

  // reads key from file and generates 256-bit key - treatment depends on
  // file format:
  // 1) file size = 32 bytes: contents are read as-is
  // 2) file size = 64 bytes and hexadecimal format: hexadecimal values are
  //    converted into bytes (32 hex pairs converted into 32 bytes)
  // 3) in all other cases, contents are hashed into a 256-bit key using SHA-256
  // <- 256-bit key
  static SecureMem<word8> GetKeyFromKeyFile(const WString& sFileName);

  // appends key from key file to password and returns resulting key combination.
  // If only one key source is provided, the result is identical to that.
  static SecureMem<word8> CombineKeySources(const SecureMem<word8>& passw,
    const WString& sKeyFileName);

  // iterators for accessing database entries
  PasswDbList::iterator begin()
  {
    return m_db.begin();
  }

  PasswDbList::iterator end()
  {
    return m_db.end();
  }

  PasswDbList::const_iterator begin() const
  {
    return m_db.cbegin();
  }

  PasswDbList::const_iterator end() const
  {
    return m_db.cend();
  }

  // operator with range check for random access to database entries
  const PasswDbEntry* operator[](word32 lIndex) const
  {
    return m_db.at(lIndex).get();
  }

  PasswDbEntry* operator[](word32 lIndex)
  {
    return m_db.at(lIndex).get();
  }

  // properties
  // default user name
  __property SecureWString DefaultUserName =
  { read=m_sDefaultUserName, write=m_sDefaultUserName };

  // default password format sequence
  __property SecureWString PasswFormatSeq =
  { read=m_sPasswFormatSeq, write=m_sPasswFormatSeq };

  // default number of expiry days for new passwords
  __property word32 DefaultPasswExpiryDays =
  { read=m_lDefaultPasswExpiryDays, write=m_lDefaultPasswExpiryDays };

  __property word32 DefaultMaxPasswHistorySize =
  { read=m_lDefaultMaxPasswHistorySize, write=m_lDefaultMaxPasswHistorySize };

  // database size
  __property word32 Size =
  { read=GetSize };

  // version number of last opened/saved database
  __property int Version =
  { read=m_nLastVersion };

  // cipher type
  __property word8 CipherType =
  { read=m_bCipherType, write=SetCipherType };

  // number of KDF iterations
  __property word32 KdfIterations =
  { read=m_lKdfIterations, write=SetKdfIterations };

  // recovery key enabled/disabled (true/false)
  __property bool HasRecoveryKey =
  { read=m_blRecoveryKey };

  // compression enabled/disabled
  __property bool Compressed =
  { read=m_blCompressed, write=m_blCompressed };

  // compression level (0 if uncompressed)
  __property int CompressionLevel =
  { read=m_nCompressionLevel, write=m_nCompressionLevel };
};


#endif
