# [Universal C++ RunTime (UCXXRT)](https://github.com/ntoskrnl7/ucxxrt)

[![Actions Status](https://github.com/MiroKaku/ucxxrt/workflows/CI/badge.svg)](https://github.com/MiroKaku/ucxxrt/actions)
[![LICENSE](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/MiroKaku/ucxxrt/blob/master/LICENSE)
[![LICENSE](https://img.shields.io/badge/license-Anti%20996-blue.svg)](https://github.com/996icu/996.ICU/blob/master/LICENSE)
![Windows](https://img.shields.io/badge/Windows-7+-orange.svg)
![Visual Studio](https://img.shields.io/badge/Visual%20Studio-2017-purple.svg)

## 1. 시작

ucxxrt는 마이크로소프트의 MSVC를 기반으로 수정된 오픈소스 런타임 라이브러리로, 가장 큰 특징은 드라이버 개발이 애플리케이션 개발과 동일한 C++ 경험을 할 수 있도록 드라이버에 사용할 수 있다는 것입니다.

ucxxrt가 탄생하기 전에는 드라이버에서 C++를 사용하여 온디맨드로 템플릿 라이브러리를 구현하는 것이었습니다.([KTL](https://github.com/MeeSong/KTL), ustd, ...)
그러나 예외가 지원되지 않는 것과 같은 몇 가지 문제가 있습니다. 가장 큰 문제는 새로운 표준이 나올 때마다 새로운 기능을 직접 구현해야 하므로 시간 낭비라는 것입니다. 그래서 ucxxrt가 탄생했습니다.

### 1.1 원리

* 드라이버 모드에서는 컴파일러가 예외를 지원할 수 있도록 속성 시트를 통해 커널 모드 플래그를 강제로 끕니다. 또한 예외 옵션(`/EHsc`)을 활성화합니다.
* 'throw' 및 'catch'와 같은 예외 기능을 구현합니다. 'throw' 함수에서 예외 처리기에 콜백을 전달하는 것을 직접 시뮬레이션합니다.

### 1.2 기능

커널 모드：

* [x] new/delete
* [x] C++ 예외처리 (/EHsc)
* [x] SAFESEH, GS (Buffer Security Check)
* [ ] 부분적인 STL
  * [ ] Thread Local Storage (TLS): thread_local、TlsAlloc ...
  * [x] std::thread
  * [ ] std::filesystem
  * [x] std::chrono
  * [ ] std::stacktrace_entry
  * [ ] std::locale
  * [ ] std::stream (std::fstream、std::iostream、std::cin、std::cout、std::cerr)
  * [x] std::mutex
  * [x] std::shared_mutex
  * [x] std::packaged_task
  * [x] std::promise
  * [x] std::future
  * [x] std::condition_variable
  * [ ] std::latch
  * [ ] std::semaphore (std::counting_semaphore、std::binary_semaphore)
  * [ ] ...
* [x] 정적 객체

### 1.3 예제

> 프로젝트 [unittest](../src/unittest.cpp) 자세한 내용을 참조하십시오.

```cpp
void Test$ThrowUnknow()
{
    try
    {
        try
        {
            try
            {
                throw std::wstring();
            }
            catch (int& e)
            {
                ASSERT(false);
                LOG(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Catch Exception: %d\n", e);
            }
        }
        catch (std::string& e)
        {
            ASSERT(false);
            LOG(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Catch Exception: %s\n", e.c_str());
        }
    }
    catch (...)
    {
        LOG(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Catch Exception: ...\n");
    }
}

void Test$HashMap()
{
    auto Rand = std::mt19937_64(::rand());
    auto Map = std::unordered_map<uint32_t, std::string>();
    for (auto i = 0u; i < 10; ++i)
    {
        Map[i] = std::to_string(Rand());
    }

    for (const auto& Item : Map)
    {
        LOG(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
            "map[%ld] = %s\n", Item.first, Item.second.c_str());
    }
}

void Test$Thread() {
    std::mutex mtx;
    std::condition_variable cv;
    std::thread t([&mtx, &cv]() {
        cv.notify_all();
        std::unique_lock<std::mutex> lk(mtx);
        });
    {
        std::unique_lock<std::mutex> lk(mtx);
        cv.wait(lk);
    }
    if (t.joinable())
        t.join();

    auto tid = std::this_thread::get_id();
    std::this_thread::sleep_for(std::chrono::seconds(1));
}
```

## 2. 빌드

IDE：Visual Studio 2017 이상

* `git clone --recursive https://github.com/ntoskrnl7/ucxxrt.git`
* `ucxxrt.sln`을 열어 빌드

## 3. 사용방법

1. 컴파일된 'unittest' 폴더를 자신의 프로젝트에 복사합니다.
2. Visual Studio에서 속성 관리자(보기 - 속성 관리자)를 열고 기존 속성 시트를 마우스 오른쪽 버튼으로 클릭하여 추가한 다음 `ucxxrt.props`를 선택합니다.

> !! [주의] 커널 모드: `DriverEntry`가 `DriverMain`으로 변경됨

![사용방법](../readme/use.gif)

## 4. 참고

* [Microsoft's C++ Standard Library](https://github.com/microsoft/stl)
* [Chuyu-Team/VC-LTL](https://github.com/Chuyu-Team/VC-LTL)
* [RetrievAL](https://github.com/SpoilerScriptsGroup/RetrievAL)

> ucxxrt를 개발할때 많은 도움이 된 위 훌륭한 프로젝트에 대단히 감사하게 생각합니다.
