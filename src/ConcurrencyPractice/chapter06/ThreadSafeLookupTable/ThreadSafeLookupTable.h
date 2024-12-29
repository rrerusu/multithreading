#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <vector>

#pragma once

template<typename Key, typename Value, typename Hash = std::hash<Key>>
class ThreadSafeLookupTable {
    private:
        class BucketType {
            private:
                typedef std::pair<Key, Value> bucketValue;
                typedef std::list<bucketValue> bucketData;
                typedef typename bucketData::iterator bucketIterator;

                bucketData data;
                mutable std::shared_mutex bucketTex;

                bucketIterator findEntryFor(Key const &);
            public:
                Value valueFor(Key const &, Value const &) const;
                void addOrUpdateMapping(Key const &, Value const &);
                void removeMapping(Key const &);
                void getData(std::map<Key, Value> &) const;
                bucketData getBucketData() const;
        };

        std::vector<std::unique_ptr<BucketType>> buckets;
        Hash hasher;

        BucketType & getBucket(Key const &) const;
    public:
        typedef Key keyType;
        typedef Value mappedType;
        typedef Hash hashType;
        
        // Default is 19 because hash table works well with prime numbers of buckets
        ThreadSafeLookupTable(unsigned numBuckets = 19, Hash const & hasher_ = Hash());
        ThreadSafeLookupTable(ThreadSafeLookupTable const &) = delete;
        ThreadSafeLookupTable(ThreadSafeLookupTable &&) = default;
        ThreadSafeLookupTable & operator=(ThreadSafeLookupTable const &) = delete;
        ThreadSafeLookupTable & operator=(ThreadSafeLookupTable &&) = default;
        ~ThreadSafeLookupTable() = default;

        Value valueFor(Key const &, Value const & defaultValue = Value()) const;
        void addOrUpdateMapping(Key const &, Value const &);
        void removeMapping(Key const &);
        std::map<Key, Value> getMap() const;
};

#include "ThreadSafeLookupTable.tpp"