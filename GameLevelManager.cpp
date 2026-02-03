#include "main.hpp"
#include "GameLevelManager.hpp"
#include "GameLevelManager.h"
#include "LocalLevelManager.h"
#include "GJGameLevel.h"
#include "GJLevelList.h"
#include "cocos2d.h"

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <sstream>

// ==============================================
// GameLevelManager - LOCAL LEVEL LOOKUPS & NAME UTILITIES
// ==============================================

namespace cocos2d {
class CCObject {
public:
    virtual ~CCObject() = default;
};

class CCString : public CCObject {
public:
    static CCString* create(const std::string& value) {
        return new CCString(value);
    }

    static CCString* createWithFormat(const char* fmt, int value) {
        char buffer[64];
        std::snprintf(buffer, sizeof(buffer), fmt, value);
        return new CCString(buffer);
    }

    const char* getCString() const {
        return m_value.c_str();
    }

    int intValue() const {
        return std::atoi(m_value.c_str());
    }

private:
    explicit CCString(std::string value) : m_value(std::move(value)) {}

    std::string m_value;
};

class CCArray : public CCObject {
public:
    unsigned int count() const {
        return static_cast<unsigned int>(m_data.size());
    }

    CCObject* objectAtIndex(unsigned int index) const {
        if (index >= m_data.size()) {
            return nullptr;
        }
        return m_data[index];
    }

    CCString* stringAtIndex(unsigned int index) const {
        return static_cast<CCString*>(objectAtIndex(index));
    }

    std::vector<CCObject*> m_data;
};

class CCDictionary : public CCObject {
public:
    void setObject(CCObject* object, const std::string& key) {
        m_stringKeys[key] = object;
    }

    void setObject(CCObject* object, int key) {
        m_intKeys[key] = object;
    }

    CCObject* objectForKey(const std::string& key) const {
        auto iter = m_stringKeys.find(key);
        return iter == m_stringKeys.end() ? nullptr : iter->second;
    }

    std::unordered_map<std::string, CCObject*> m_stringKeys;
    std::unordered_map<int, CCObject*> m_intKeys;
};
} // namespace cocos2d

class GJGameLevel : public cocos2d::CCObject {
public:
    int m_levelID = 0;
    std::string m_levelName;
};

class GJLevelList : public cocos2d::CCObject {
public:
    int m_listID = 0;
};

class LocalLevelManager {
public:
    static LocalLevelManager* sharedState();
    cocos2d::CCDictionary* getAllLevelsInDict();
};

class GameLevelManager {
public:
    GJGameLevel* getLocalLevel(int levelID);
    std::string getNextLevelName(const std::string& name);
    GJLevelList* getLocalLevelList(int listID);
    void storeUserName(int userID, int accountID, const std::string& name);
    void storeUserNames(const std::string& encodedUsernames);
    std::string userNameForUserID(int userID);

private:
    cocos2d::CCArray* m_localLevels = nullptr;       // 0x150
    cocos2d::CCArray* m_localLevelLists = nullptr;   // 0x158
    cocos2d::CCDictionary* m_userNameDict = nullptr; // 0x248 (userID -> name)
    cocos2d::CCDictionary* m_accountIdDict = nullptr; // 0x250 (accountID -> userID)
    cocos2d::CCDictionary* m_userIdDict = nullptr;   // 0x258 (userID -> accountID)
};

GJGameLevel* GameLevelManager::getLocalLevel(int levelID) {
    LocalLevelManager::sharedState();
    if (!m_localLevels) {
        return nullptr;
    }

    for (unsigned int index = 0; index < m_localLevels->count(); ++index) {
        auto* level = static_cast<GJGameLevel*>(m_localLevels->objectAtIndex(index));
        if (level && level->m_levelID == levelID) {
            return level;
        }
    }

    return nullptr;
}

std::string GameLevelManager::getNextLevelName(const std::string& name) {
    if (name.empty()) {
        return name;
    }

    size_t digits = 0;
    for (size_t index = name.size(); index > 0; --index) {
        char character = name[index - 1];
        if (!std::isdigit(static_cast<unsigned char>(character))) {
            break;
        }
        ++digits;
    }

    if (digits == 0) {
        if (name.size() <= 18) {
            return name + " Copy";
        }
        return name;
    }

    LocalLevelManager* localLevels = LocalLevelManager::sharedState();
    cocos2d::CCDictionary* levelDict = localLevels ? localLevels->getAllLevelsInDict() : nullptr;

    std::string baseName = name.substr(0, name.size() - digits);
    int currentNumber = std::atoi(name.substr(name.size() - digits).c_str());
    int limit = currentNumber + 1000;

    int nextNumber = currentNumber;
    std::string candidate;
    do {
        ++nextNumber;
        candidate = baseName + std::to_string(nextNumber);

        if (!levelDict || !levelDict->objectForKey(candidate)) {
            return candidate;
        }
    } while (nextNumber != limit);

    return name;
}

GJLevelList* GameLevelManager::getLocalLevelList(int listID) {
    LocalLevelManager::sharedState();
    if (!m_localLevelLists) {
        return nullptr;
    }

    for (unsigned int index = 0; index < m_localLevelLists->count(); ++index) {
        auto* list = static_cast<GJLevelList*>(m_localLevelLists->objectAtIndex(index));
        if (list && list->m_listID == listID) {
            return list;
        }
    }

    return nullptr;
}

void GameLevelManager::storeUserName(int userID, int accountID, const std::string& name) {
    if (userID <= 0) {
        return;
    }

    if (!m_userNameDict) {
        m_userNameDict = new cocos2d::CCDictionary();
    }

    auto* userName = cocos2d::CCString::create(name);
    std::string userIDKey = std::to_string(userID);
    m_userNameDict->setObject(userName, userIDKey);

    if (accountID > 0) {
        if (!m_userIdDict) {
            m_userIdDict = new cocos2d::CCDictionary();
        }
        if (!m_accountIdDict) {
            m_accountIdDict = new cocos2d::CCDictionary();
        }

        auto* accountIdString = cocos2d::CCString::createWithFormat("%i", accountID);
        m_userIdDict->setObject(accountIdString, userID);

        auto* userIdString = cocos2d::CCString::createWithFormat("%i", userID);
        m_accountIdDict->setObject(userIdString, accountID);
    }
}

void GameLevelManager::storeUserNames(const std::string& encodedUsernames) {
    if (encodedUsernames.empty()) {
        return;
    }

    std::stringstream stream(encodedUsernames);
    std::string entry;

    while (std::getline(stream, entry, '|')) {
        if (entry.empty()) {
            continue;
        }

        std::stringstream entryStream(entry);
        std::string token;
        std::vector<std::string> parts;

        while (std::getline(entryStream, token, ':')) {
            parts.push_back(token);
        }

        if (parts.size() < 3) {
            continue;
        }

        int userID = std::atoi(parts[0].c_str());
        const std::string& name = parts[1];
        int accountID = std::atoi(parts[2].c_str());

        storeUserName(userID, accountID, name);
    }
}

std::string GameLevelManager::userNameForUserID(int userID) {
    if (userID <= 0 || !m_userNameDict) {
        return "";
    }

    std::string key = std::to_string(userID);
    auto* value = static_cast<cocos2d::CCString*>(m_userNameDict->objectForKey(key));
    if (!value) {
        return "";
    }

    return value->getCString();
}
