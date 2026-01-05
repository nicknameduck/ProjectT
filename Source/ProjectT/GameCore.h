#pragma once
#include "EngineMinimal.h"
#include "EngineUtils.h"


#if !UE_BUILD_SHIPPING

#define PT_LOG_ALERT(Condition, Verbosity, Format, ...)                                     \
    {                                                                                       \
        if (!(Condition))                                                                   \
        {                                                                                   \
            FString Msg = FString::Printf(Format, ##__VA_ARGS__);                           \
                                                                                            \
            FString Location = FString::Printf(                                             \
                TEXT("File: %s (%d)\nFunction: %s"),                                        \
                TEXT(__FILE__),                                                             \
                __LINE__,                                                                   \
                TEXT(__FUNCTION__)                                                          \
            );                                                                              \
                                                                                            \
            /* 로그 */                                                                       \
            UE_LOG(ProjectTLog, Verbosity, TEXT("%s\n%s"), *Msg, *Location);                \
                                                                                            \
            /* OnScreen 메시지 */                                                            \
            if (GEngine)                                                                    \
            {                                                                               \
                GEngine->AddOnScreenDebugMessage(                                           \
                    -1, 10.f, FColor::Red,                                                  \
                    Msg + TEXT("\n\n") + Location                                          \
                );                                                                          \
            }                                                                               \
                                                                                            \
            /* 팝업 */                                                                      \
            FMessageDialog::Open(                                                           \
                EAppMsgType::Ok,                                                            \
                FText::FromString(Msg + TEXT("\n\n") + Location)                            \
            );                                                                              \
        }                                                                                   \
    }

#else

#define PT_LOG_ALERT(Condition, Verbosity, Format, ...)                                     \
    {                                                                                       \
        if (!(Condition))                                                                   \
        {                                                                                   \
            FString Msg = FString::Printf(Format, ##__VA_ARGS__);                           \
            UE_LOG(ProjectTLog, Verbosity, TEXT("%s"), *Msg);                               \
        }                                                                                   \
    }

#endif

//#if !UE_BUILD_SHIPPING
//
//#define PT_LOG_ALERT(Condition, Verbosity, Format, ...)                             \
//    {                                                                               \
//        if (!(Condition))                                                           \
//        {                                                                           \
//            /* 기본 메시지 */                                                      \
//            FString Msg = FString::Printf(Format, ##__VA_ARGS__);                   \
//                                                                                    \
//            /* C++ Callstack */                                                     \
//            ANSICHAR StackTrace[65536];                                             \
//            FPlatformStackWalk::StackWalkAndDump(                                   \
//                StackTrace,                                                         \
//                UE_ARRAY_COUNT(StackTrace),                                         \
//                0                                                                   \
//            );                                                                      \
//            FString Callstack = ANSI_TO_TCHAR(StackTrace);                          \
//                                                                                    \
//            /* 화면 로그 */                                                         \
//            UE_LOG(ProjectTLog, Verbosity, TEXT("%s"), *Msg);                       \
//            UE_LOG(ProjectTLog, Verbosity, TEXT("Callstack:\n%s"), *Callstack);      \
//                                                                                    \
//            /* 화면 OnScreen 메시지 */                                             \
//            if (GEngine)                                                            \
//            {                                                                       \
//                GEngine->AddOnScreenDebugMessage(                                   \
//                    -1, 10.f, FColor::Red,                                          \
//                    Msg + TEXT("\n\n--- Callstack ---\n") + Callstack               \
//                );                                                                  \
//            }                                                                       \
//                                                                                    \
//            /* 팝업 메시지 (Dialog) */                                              \
//            {                                                                       \
//                FString FullMsg = Msg + TEXT("\n\n--- Callstack ---\n") + Callstack;\
//                FMessageDialog::Open(                                               \
//                    EAppMsgType::Ok,                                                \
//                    FText::FromString(FullMsg)                                      \
//                );                                                                  \
//            }                                                                       \
//        }                                                                           \
//    }
//
//#else
//
//#define PT_LOG_ALERT(Condition, Verbosity, Format, ...)                             \
//    {                                                                               \
//        if (!(Condition))                                                           \
//        {                                                                           \
//            FString Msg = FString::Printf(Format, ##__VA_ARGS__);                   \
//            UE_LOG(ProjectTLog, Verbosity, TEXT("%s"), *Msg);                       \
//        }                                                                           \
//    }
//
//#endif

DECLARE_LOG_CATEGORY_EXTERN(ProjectTLog, Warning, All);

#define	TeamNeutral	255
#define	TeamMonster	30
#define	TeamPlayer	1

