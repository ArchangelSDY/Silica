#ifndef FREQUENCYCACHE_H
#define FREQUENCYCACHE_H

#include <QHash>

template <class K, class V>
class FrequencyCache {
public:
    FrequencyCache(int maxCount);
    ~FrequencyCache();

    void insert(const K &key, const V &);
    V find(const K &key);
    void clear();

private:
    void trim();

    int m_capacity;
    QHash<K, int> m_frequency;
    QHash<K, V> m_hash;
    QMutex m_mutex;
};

template <class K, class V>
FrequencyCache<K, V>::FrequencyCache(int capacity) :
    m_capacity(capacity)
{
}

template <class K, class V>
FrequencyCache<K, V>::~FrequencyCache()
{
    clear();
}

template <class K, class V>
void FrequencyCache<K, V>::clear()
{
    QMutexLocker locker(&m_mutex);

    m_hash.clear();
    m_frequency.clear();
}

template <class K, class V>
void FrequencyCache<K, V>::trim()
{
    if (m_hash.size() >= m_capacity) {
        typename QHash<K, int>::const_iterator leastVisited =
            m_frequency.begin();
        for (typename QHash<K, int>::const_iterator it = m_frequency.begin();
            it != m_frequency.end(); ++it) {
            if (it.value() < leastVisited.value()) {
                 leastVisited = it;
             }
        }

        QMutexLocker locker(&m_mutex);

        m_hash.remove(leastVisited.key());
        m_frequency.remove(leastVisited.key());
    }
}

template <class K, class V>
void FrequencyCache<K, V>::insert(const K &key, const V &value)
{
    if (m_hash.contains(key)) {
        typename QHash<K, int>::iterator hit = m_frequency.find(key);
        hit.value()++;
    } else {
        trim();
        m_hash.insert(key, value);
        m_frequency.insert(key, 1);
    }
}

template <class K, class V>
V FrequencyCache<K, V>::find(const K &key)
{
    return m_hash.value(key);
}

#endif // FREQUENCYCACHE_H
