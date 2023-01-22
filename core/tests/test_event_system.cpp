#include <gtest/gtest.h>
#include <cubos/core/log.hpp>
#include <cubos/core/ecs/event_pipe.hpp>
#include <cubos/core/ecs/event_writer.hpp>
#include <cubos/core/ecs/event_reader.hpp>

using namespace cubos::core::ecs;
using namespace cubos::core::memory;

TEST(Cubos_Core_Event_System, Event_System_Simple_Test)
{
    auto pipe = EventPipe<int>();
    auto writer = EventWriter<int>(pipe);
    std::size_t index = 0;

    std::size_t size = 0;
    for (auto& x : EventReader<int>(pipe, index))
    {
        size++;
    }
    EXPECT_EQ(size, 0);

    index = 0;
    writer.push(3); // push (3,0)
    for (size = 0; auto& x : EventReader<int>(pipe, index))
    {
        size++;
    }
    EXPECT_EQ(size, 1);

    index = 0;
    writer.push(3, 5);
    for (size = 0; auto& x : EventReader<int>(pipe, index))
    {
        size++;
    }
    EXPECT_EQ(size, 2);

    index = 0;
    pipe.clear();
    for (size = 0; auto& x : EventReader<int>(pipe, index))
    {
        size++;
    }
    EXPECT_EQ(size, 2);
}

TEST(Cubos_Core_Event_System, Event_Pipe_Writer_Simple_Test)
{
    auto pipe = EventPipe<int>();
    auto writer = EventWriter<int>(pipe);

    EXPECT_EQ(pipe.sentEvents(), 0);

    writer.push(3);
    EXPECT_EQ(pipe.sentEvents(), 1);

    writer.push(3, 5);
    EXPECT_EQ(pipe.sentEvents(), 2);

    pipe.clear();
    EXPECT_EQ(pipe.sentEvents(), 2);

    writer.push(3, 5);
    EXPECT_EQ(pipe.sentEvents(), 3);
}

TEST(Cubos_Core_Event_System, Event_System_Masking_Test)
{
    struct MyEvent
    {
        enum Mask
        {
            KEY_EVENT = 1 << 0,   // 0001
            MOUSE_EVENT = 1 << 1, // 0010
            WHEEL_EVENT = 1 << 2, // 0100
            ALL = (1 << 3) - 1,   // 0111
        };

        int data; // random data member
    };

    auto pipe = EventPipe<MyEvent>();
    auto writer = EventWriter<MyEvent>(pipe);

    writer.push(MyEvent{.data = 1}, MyEvent::Mask::KEY_EVENT);
    writer.push(MyEvent{.data = 2}, MyEvent::Mask::WHEEL_EVENT);
    writer.push(MyEvent{.data = 3}, MyEvent::Mask::MOUSE_EVENT);
    writer.push(MyEvent{.data = 4}, MyEvent::Mask::MOUSE_EVENT);
    writer.push(MyEvent{.data = 5});
    writer.push(MyEvent{.data = 6});

    std::size_t index = 0;

    std::size_t mouseReaderSize = 0;
    auto mouseReader = EventReader<MyEvent, MyEvent::Mask::MOUSE_EVENT>(pipe, index);
    pipe.addReader();
    while (true)
    {
        auto it = mouseReader.read();
        if (it == std::nullopt)
        {
            break;
        }
        mouseReaderSize++;
    }
    EXPECT_EQ(mouseReaderSize, 2);
    pipe.removeReader();

    index = 0;
    auto keyEventReader = EventReader<MyEvent, MyEvent::Mask::KEY_EVENT>(pipe, index);
    pipe.addReader();
    for (const auto& ev : keyEventReader)
    {
        // should run only once
        EXPECT_EQ(ev.data, 1);
    }
    for (const auto& ev : keyEventReader)
    {
        // this should NOT run, because all KEY_EVENTS from keyEventReader already got read
        FAIL();
    }
    pipe.removeReader();
}