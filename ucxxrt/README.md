# [Universal C++ RunTime (UCXXRT)](https://github.com/mirokaku/ucxxrt)

[![Actions Status](https://github.com/MiroKaku/ucxxrt/workflows/CI/badge.svg)](https://github.com/MiroKaku/ucxxrt/actions)
[![LICENSE](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/MiroKaku/ucxxrt/blob/master/LICENSE)
[![LICENSE](https://img.shields.io/badge/license-Anti%20996-blue.svg)](https://github.com/996icu/996.ICU/blob/master/LICENSE)
![Windows](https://img.shields.io/badge/Windows-7+-orange.svg)
![Visual Studio](https://img.shields.io/badge/Visual%20Studio-2017-purple.svg)

* [简体中文](./docs/zh-cn.md), [한국어](./docs/ko-kr.md)

## 1. About

ucxxrt is a open source rutime library which based on MSVC.The highlight of this project is that it can be used in kernel-mode drivers.  
It gives you the same experience as user-mode application development in C++ when developing kernel-mode drivers.

Before ucxxrt was born,in order to use C++ on kernel-mode drivers, I use  ([KTL](https://github.com/MeeSong/KTL)、ustd、...).  

But there are several problems，like it isn't support C++ exception and it cost much time on implementing new features which provided by the latest ISO,then ucxxrt was born.  

### 1.1 Principle  

* In kernel-mode driver mode,forced disable kernel-mode flag by using property sheet ,it makes the compiler support C++ exceptions.
* Implement the exception functions such as `throw`、`catch`.  Simulated the exception dispatcher in `throw`.  

### 1.2 Features

Kernel-mode：

* [x] support new/delete operators.
* [x] support C++ exception (/EHsc).  
* [x] support SAFESEH、GS (Buffer Security Check).  
* [x] support STL (not fully).  
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
* [x] support static objects.  

[List of features that are not supported at this time↓](#5-List-of-features-that-are-not-supported-at-this-time)

### 1.3 Example

> See project [unittest](https://github.com/MiroKaku/ucxxrt/blob/master/src/unittest.cpp) for more Infomation.  

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

## 2. Compile

IDE：Visual Studio 2017 or higher

* `git clone --recursive https://github.com/ntoskrnl7/ucxxrt.git`
* Open `ucxxrt.sln` and compile

## 3. How to use

1. Copy `ucxxrt/ucxxrt` forder to you project directory.  
2. Add the property sheet `ucxxrt.props` to yor project.  

> !! kernel-mode：Rename `DriverEntry` to `DriverMain`  

![usage](./readme/use.gif)

## 4. Reference and Acknowledgement

* [Microsoft's C++ Standard Library](https://github.com/microsoft/stl)
* [Chuyu-Team/VC-LTL](https://github.com/Chuyu-Team/VC-LTL)
* [RetrievAL](https://github.com/SpoilerScriptsGroup/RetrievAL)

> Thanks to these excellent projects for help me on developing ucxxrt.
