

#ifndef MessageQueue_h
#define MessageQueue_h

#include <limits>
#include <wtf/Assertions.h>
#include <wtf/Deque.h>
#include <wtf/Noncopyable.h>
#include <wtf/Threading.h>

namespace WTF {

    enum MessageQueueWaitResult {
        MessageQueueTerminated,       // Queue was destroyed while waiting for message.
        MessageQueueTimeout,          // Timeout was specified and it expired.
        MessageQueueMessageReceived,  // A message was successfully received and returned.
    };

    // The queue takes ownership of messages and transfer it to the new owner
    // when messages are fetched from the queue.
    // Essentially, MessageQueue acts as a queue of OwnPtr<DataType>.
    template<typename DataType>
    class MessageQueue : public Noncopyable {
    public:
        MessageQueue() : m_killed(false) { }
        ~MessageQueue();

        void append(PassOwnPtr<DataType>);
        bool appendAndCheckEmpty(PassOwnPtr<DataType>);
        void prepend(PassOwnPtr<DataType>);

        PassOwnPtr<DataType> waitForMessage();
        PassOwnPtr<DataType> tryGetMessage();
        template<typename Predicate>
        PassOwnPtr<DataType> waitForMessageFilteredWithTimeout(MessageQueueWaitResult&, Predicate&, double absoluteTime);

        template<typename Predicate>
        void removeIf(Predicate&);

        void kill();
        bool killed() const;

        // The result of isEmpty() is only valid if no other thread is manipulating the queue at the same time.
        bool isEmpty();

        static double infiniteTime() { return std::numeric_limits<double>::max(); }

    private:
        static bool alwaysTruePredicate(DataType*) { return true; }

        mutable Mutex m_mutex;
        ThreadCondition m_condition;
        Deque<DataType*> m_queue;
        bool m_killed;
    };

    template<typename DataType>
    MessageQueue<DataType>::~MessageQueue()
    {
        deleteAllValues(m_queue);
    }

    template<typename DataType>
    inline void MessageQueue<DataType>::append(PassOwnPtr<DataType> message)
    {
        MutexLocker lock(m_mutex);
        m_queue.append(message.release());
        m_condition.signal();
    }

    // Returns true if the queue was empty before the item was added.
    template<typename DataType>
    inline bool MessageQueue<DataType>::appendAndCheckEmpty(PassOwnPtr<DataType> message)
    {
        MutexLocker lock(m_mutex);
        bool wasEmpty = m_queue.isEmpty();
        m_queue.append(message.release());
        m_condition.signal();
        return wasEmpty;
    }

    template<typename DataType>
    inline void MessageQueue<DataType>::prepend(PassOwnPtr<DataType> message)
    {
        MutexLocker lock(m_mutex);
        m_queue.prepend(message.release());
        m_condition.signal();
    }

    template<typename DataType>
    inline PassOwnPtr<DataType> MessageQueue<DataType>::waitForMessage()
    {
        MessageQueueWaitResult exitReason; 
        PassOwnPtr<DataType> result = waitForMessageFilteredWithTimeout(exitReason, MessageQueue<DataType>::alwaysTruePredicate, infiniteTime());
        ASSERT(exitReason == MessageQueueTerminated || exitReason == MessageQueueMessageReceived);
        return result;
    }

    template<typename DataType>
    template<typename Predicate>
    inline PassOwnPtr<DataType> MessageQueue<DataType>::waitForMessageFilteredWithTimeout(MessageQueueWaitResult& result, Predicate& predicate, double absoluteTime)
    {
        MutexLocker lock(m_mutex);
        bool timedOut = false;

        DequeConstIterator<DataType*> found = m_queue.end();
        while (!m_killed && !timedOut && (found = m_queue.findIf(predicate)) == m_queue.end())
            timedOut = !m_condition.timedWait(m_mutex, absoluteTime);

        ASSERT(!timedOut || absoluteTime != infiniteTime());

        if (m_killed) {
            result = MessageQueueTerminated;
            return 0;
        }

        if (timedOut) {
            result = MessageQueueTimeout;
            return 0;
        }

        ASSERT(found != m_queue.end());
        DataType* message = *found;
        m_queue.remove(found);
        result = MessageQueueMessageReceived;
        return message;
    }

    template<typename DataType>
    inline PassOwnPtr<DataType> MessageQueue<DataType>::tryGetMessage()
    {
        MutexLocker lock(m_mutex);
        if (m_killed)
            return 0;
        if (m_queue.isEmpty())
            return 0;

        DataType* message = m_queue.first();
        m_queue.removeFirst();
        return message;
    }

    template<typename DataType>
    template<typename Predicate>
    inline void MessageQueue<DataType>::removeIf(Predicate& predicate)
    {
        MutexLocker lock(m_mutex);
        // See bug 31657 for why this loop looks so weird
        while (true) {
            DequeConstIterator<DataType*> found = m_queue.findIf(predicate);
            if (found == m_queue.end())
                break;

            DataType* message = *found;
            m_queue.remove(found);
            delete message;
       }
    }

    template<typename DataType>
    inline bool MessageQueue<DataType>::isEmpty()
    {
        MutexLocker lock(m_mutex);
        if (m_killed)
            return true;
        return m_queue.isEmpty();
    }

    template<typename DataType>
    inline void MessageQueue<DataType>::kill()
    {
        MutexLocker lock(m_mutex);
        m_killed = true;
        m_condition.broadcast();
    }

    template<typename DataType>
    inline bool MessageQueue<DataType>::killed() const
    {
        MutexLocker lock(m_mutex);
        return m_killed;
    }
} // namespace WTF

using WTF::MessageQueue;
// MessageQueueWaitResult enum and all its values.
using WTF::MessageQueueWaitResult;
using WTF::MessageQueueTerminated;
using WTF::MessageQueueTimeout;
using WTF::MessageQueueMessageReceived;

#endif // MessageQueue_h
