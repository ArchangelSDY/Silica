#ifndef CONCURRENTHASH_H
#define CONCURRENTHASH_H

#include <QHash>
#include <QMutex>
#include <QMutexLocker>

template <class K, class V>
class ConcurrentHash
{
public:
    void insert(const K &key, const V &);
    bool contains(const K &key) const;
    int remove(const K &key);

private:
    QHash<K, V> m_hash;
    QMutex m_mutex;
};

template <class K, class V>
void ConcurrentHash<K, V>::insert(const K &key, const V &value)
{
    QMutexLocker locker(&m_mutex);
    m_hash.insert(key, value);
}

template <class K, class V>
bool ConcurrentHash<K, V>::contains(const K &key) const
{
    return m_hash.contains(key);
}

template <class K, class V>
int ConcurrentHash<K, V>::remove(const K &key)
{
    QMutexLocker locker(&m_mutex);
    return m_hash.remove(key);
}

#endif // CONCURRENTHASH_H
