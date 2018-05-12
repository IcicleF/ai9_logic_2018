//
// Created by IcyF on 2018/2/7.
//

/* Macro definitions copied from UI_Logic_API.h, Ai9_logic_2017.
 * This comment is to express thanks to the author of the header file above.
 */

#ifndef UIAPI_H
#define UIAPI_H

#if defined(_MSC_VER)

#define EXPORT __declspec(dllexport)
#define IMPORT __declspec(dllimport)

#elif defined(__GNUC__)

#define EXPORT __attribute__((visibility("default")))
#define IMPORT

#else

#define EXPORT
#define IMPORT
#pragma warning Unknown dynamic link import/export semantics.

#endif

#ifdef __cplusplus
extern "C" {
#endif

EXPORT void init(int);

EXPORT void setReplayFile(const char*);

EXPORT void getPlayerIDs(int*);

EXPORT void setPlayerName(int, const char*);

EXPORT bool loadAI(const char*, int);

EXPORT void beginJudge();

EXPORT bool invokeAI();

EXPORT void run();

EXPORT void getCommands(char*);

EXPORT int checkWin();

EXPORT void endJudge();

EXPORT void getRank(int*, int*);

EXPORT void closeReplayFile();

#ifdef __cplusplus
}
#endif

#endif //UIAPI_H
