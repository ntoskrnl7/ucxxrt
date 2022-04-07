#ifdef __KERNEL_MODE
#include <ntddk.h>
#include <wdm.h>
#else
#include <windows.h>
#include <assert.h>
#endif

#include <ucxxrt.h>

#include <string>
#include <random>
#include <vector>
#include <functional>
#include <unordered_map>
#include <stdexcept>

#ifdef _KERNEL_MODE
#define LOG(_0, _1, ...) DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, __VA_ARGS__)
#else
#define LOG(_0, _1, ...) printf(__VA_ARGS__)
#endif

static std::vector<std::function<void()>> TestVec;
#define TEST(f) TestVec.emplace_back(f)
#ifndef ASSERT
#  define ASSERT assert
#endif

class Test$StaticObject
{
    ULONG* _Data;

public:

    Test$StaticObject()
        : _Data(new ULONG[1]{ 1 })
    {
        LOG(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Static Object: " __FUNCTION__ "\n");
    }

    ~Test$StaticObject()
    {
        LOG(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Static Object: " __FUNCTION__ "\n");

        ASSERT(_Data[0] == 1);
        delete[] _Data;
    }
};

static Test$StaticObject _test;

void Test$Float2Int()
{
    // On x86, call _ftoui/_ftoui2/_ftoui3

    float f = 1.6f;
    auto  i = (uint32_t)f;

    ASSERT(i == 1);
    UNREFERENCED_PARAMETER(i);
}

void Test$ThrowInt()
{
    try
    {
        try
        {
            try
            {
                throw 1;
            }
            catch (int& e)
            {
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
        ASSERT(false);
        LOG(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Catch Exception: ...\n");
    }
}

void Test$ThrowObject()
{
    try
    {
        try
        {
            try
            {
                throw std::string("123");
            }
            catch (int& e)
            {
                ASSERT(false);
                LOG(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Catch Exception: %d\n", e);
            }
        }
        catch (std::string& e)
        {
            LOG(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Catch Exception: %s\n", e.c_str());
        }
    }
    catch (...)
    {
        ASSERT(false);
        LOG(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Catch Exception: ...\n");
    }
}

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

template<typename T>
class Test$InitializerListObject
{
    using iterator               = typename std::vector<T>::iterator;
    using const_iterator         = typename std::vector<T>::const_iterator;
    using reverse_iterator       = typename std::vector<T>::reverse_iterator;
    using const_reverse_iterator = typename std::vector<T>::const_reverse_iterator;

    std::vector<T> _vec;

public:
    Test$InitializerListObject(std::initializer_list<T> x)
        : _vec(x)
    {}

    [[nodiscard]] const_iterator begin() const noexcept {
        return _vec.begin();
    }

    [[nodiscard]] iterator end() noexcept {
        return _vec.end();
    }

    [[nodiscard]] const_iterator cbegin() const noexcept {
        return _vec.cbegin();
    }

    [[nodiscard]] const_iterator cend() const noexcept {
        return _vec.cend();
    }
};

void Test$InitializerList()
{
    auto v = Test$InitializerListObject{ 0, 1, 2, 3, 4 };

    int x = 0;
    for (const auto i : v)
    {
        ASSERT(i == x); ++x;
    }
}

std::unordered_map<std::string, ULONG_PTR> Test$StaticObjectInitializer =
{
    { "1", 1 },
    { "2", 2 },
    { "3", 3 },
    { "4", 4 },
    { "5", 5 },
};

#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

void Test$Thread() {
    using namespace std::chrono_literals;

    std::mutex mtx;
    std::condition_variable cv;

    bool ready = false;
    std::thread t([&cv, &ready]() {
        std::this_thread::sleep_for(3s);
        ready = true;
        cv.notify_all();
        std::this_thread::sleep_for(3s);
        });

    {
        std::unique_lock<std::mutex> lk(mtx);
        cv.wait(lk, [&ready] { return ready; });
    }

    if (t.joinable())
        t.join();

    ASSERT(t.get_id() != std::this_thread::get_id());

    std::this_thread::sleep_for(3s);
}



//
// https://en.cppreference.com/w/cpp/thread/condition_variable
//
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>

std::mutex m;
std::condition_variable cv;
std::string data;
bool ready = false;
bool processed = false;

void worker_thread()
{
    // Wait until main() sends data
    std::unique_lock lk(m);
    cv.wait(lk, [] {return ready; });

    // after the wait, we own the lock.
    
    LOG(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Worker thread is processing data\n");
    data += " after processing";

    // Send data back to main()
    processed = true;
    LOG(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Worker thread signals data processing completed\n");

    // Manual unlocking is done before notifying, to avoid waking up
    // the waiting thread only to block again (see notify_one for details)
    lk.unlock();
    cv.notify_one();
}

void Test$ConditionVariable()
{
    std::thread worker(worker_thread);

    data = "Example data";
    // send data to the worker thread
    {
        std::lock_guard lk(m);
        ready = true;
        LOG(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Test$ConditionVariable() signals data ready for processing\n");
    }
    cv.notify_one();

    // wait for the worker
    {
        std::unique_lock lk(m);
        cv.wait(lk, [] {return processed; });
    }
    LOG(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Back in Test$ConditionVariable(), data = %s\n", data.c_str());
    worker.join();
}



//
// https://en.cppreference.com/w/cpp/thread/mutex
//
#include <iostream>
#include <map>
#include <string>
#include <chrono>
#include <thread>
#include <mutex>

std::map<std::string, std::string> g_pages;
std::mutex g_pages_mutex;

void save_page(const std::string& url)
{
    // simulate a long page fetch
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::string result = "fake content";

    std::lock_guard<std::mutex> guard(g_pages_mutex);
    g_pages[url] = result;
}

void Test$Mutex() {
    std::thread t1(save_page, "http://foo");
    std::thread t2(save_page, "http://bar");
    t1.join();
    t2.join();

    // safe to access g_pages without lock now, as the threads are joined
    for (const auto& pair : g_pages) {
        LOG(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
            "%s => %s\n", pair.first.c_str(), pair.second.c_str());
    }
}



//
// https://en.cppreference.com/w/cpp/thread/shared_mutex
//
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <thread>

class ThreadSafeCounter {
public:
    ThreadSafeCounter() = default;

    // Multiple threads/readers can read the counter's value at the same time.
    unsigned int get() const {
        std::shared_lock lock(mutex_);
        return value_;
    }

    // Only one thread/writer can increment/write the counter's value.
    unsigned int increment() {
        std::unique_lock lock(mutex_);
        return ++value_;
    }

    // Only one thread/writer can reset/write the counter's value.
    void reset() {
        std::unique_lock lock(mutex_);
        value_ = 0;
    }

private:
    mutable std::shared_mutex mutex_;
    unsigned int value_ = 0;
};

void Test$SharedMutex() {
    ThreadSafeCounter counter;

    auto increment_and_print = [&counter]() {
        for (int i = 0; i < 3; i++) {
            LOG(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
                "%p %s\n", std::this_thread::get_id(), counter.increment());

            // Note: Writing to std::cout actually needs to be synchronized as well
            // by another std::mutex. This has been omitted to keep the example small.
        }
    };

    std::thread thread1(increment_and_print);
    std::thread thread2(increment_and_print);

    thread1.join();
    thread2.join();
}



#ifdef _KERNEL_MODE
EXTERN_C NTSTATUS DriverMain(PDRIVER_OBJECT aDriverObject, PUNICODE_STRING /*aRegistry*/)
#else
EXTERN_C int main()
#endif
{
    TEST(Test$Float2Int);
    TEST(Test$ThrowInt);
    TEST(Test$ThrowObject);
    TEST(Test$ThrowUnknow);
    TEST(Test$HashMap);
    TEST(Test$InitializerList);
    TEST(Test$Thread);
    TEST(Test$ConditionVariable);
    TEST(Test$Mutex);
    TEST(Test$SharedMutex);

    for (const auto& Test : TestVec)
    {
        Test();
    }

#ifdef _KERNEL_MODE
    aDriverObject->DriverUnload = [](PDRIVER_OBJECT)
    {
        LOG(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Exit ucxxrt-test.\n");
    };
#endif

    return 0l;
}
