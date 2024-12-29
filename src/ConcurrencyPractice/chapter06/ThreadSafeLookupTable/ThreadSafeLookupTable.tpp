#include <algorithm>

#include "ThreadSafeLookupTable.h"

template<typename Key, typename Value, typename Hash>
typename ThreadSafeLookupTable<Key, Value, Hash>::BucketType::bucketIterator ThreadSafeLookupTable<Key, Value, Hash>::BucketType::findEntryFor(Key const & key) {
    return std::find_if(data.begin(), data.end(), [&](bucketValue const & item){ return item.first == key; });
}

template<typename Key, typename Value, typename Hash>
Value ThreadSafeLookupTable<Key, Value, Hash>::BucketType::valueFor(Key const & key, Value const & defaultValue) const {
    std::shared_lock<std::shared_mutex> lookupLock(bucketTex);
    bucketIterator const foundEntry = findEntryFor(key);
    return (foundEntry == data.end()) ? defaultValue : foundEntry->second;
}

template<typename Key, typename Value, typename Hash>
void ThreadSafeLookupTable<Key, Value, Hash>::BucketType::addOrUpdateMapping(Key const & key, Value const & value) {
    std::unique_lock<std::shared_mutex> lookupLock(bucketTex);
    bucketIterator const foundEntry = findEntryFor(key);

    if(foundEntry == data.end())
        data.push_back(bucketValue(key, value));
    else
        foundEntry->second = value;
}

template<typename Key, typename Value, typename Hash>
void ThreadSafeLookupTable<Key, Value, Hash>::BucketType::removeMapping(Key const & key) {
    std::unique_lock<std::shared_mutex> lookupLock(bucketTex);
    bucketIterator const foundEntry = findEntryFor(key);
    if(foundEntry != data.end())
        data.erase(foundEntry);
}

template<typename Key, typename Value, typename Hash>
void ThreadSafeLookupTable<Key, Value, Hash>::BucketType::getData(std::map<Key, Value> & result) const {
    std::shared_lock<std::shared_mutex> lock(bucketTex);
    for(bucketValue entry : data)
        result.insert(entry);
}

template<typename Key, typename Value, typename Hash>
typename ThreadSafeLookupTable<Key, Value, Hash>::BucketType & ThreadSafeLookupTable<Key, Value, Hash>::getBucket(Key const & key) const {
    std::size_t const bucketIndex = hasher(key) % buckets.size();
    return *buckets[bucketIndex];
}

template<typename Key, typename Value, typename Hash>
ThreadSafeLookupTable<Key, Value, Hash>::ThreadSafeLookupTable(unsigned numBuckets, Hash const & hasher_) :
    buckets(numBuckets), hasher(hasher) {
    for(unsigned i = 0; i < numBuckets; i++)
        buckets[i].reset(new BucketType());
}

template<typename Key, typename Value, typename Hash>
Value ThreadSafeLookupTable<Key, Value, Hash>::valueFor(Key const & key, Value const & defaultValue) const {
    return getBucket(key).valueFor(key, defaultValue);
}

template<typename Key, typename Value, typename Hash>
void ThreadSafeLookupTable<Key, Value, Hash>::addOrUpdateMapping(Key const & key, Value const & value) {
    getBucket(key).addOrUpdateMapping(key, value);
}

template<typename Key, typename Value, typename Hash>
void ThreadSafeLookupTable<Key, Value, Hash>::removeMapping(Key const & key) {
    getBucket(key).removeMapping(key);
}

template<typename Key, typename Value, typename Hash>
std::map<Key, Value> ThreadSafeLookupTable<Key, Value, Hash>::getMap() const {
    std::map<Key, Value> result;
    for(const auto & bucket : buckets) {
        bucket->getData(result);
    }
    return result;
}